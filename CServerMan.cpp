#include "eiface.h"
#include "edict.h"
#include "CServerMan.h"
#include "CPlayer.h"
#include "igameevents.h"
#include "CTimer.h"
#include "addict_globals.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//Get any external tools we need from plugin
extern IVEngineServer	* engine;
extern CTimerMan * pTimerMan;
extern userOptions * pCurrentSettings;
//extern IUniformRandomStream * randomStr;

// useful helper func
inline bool FStrEq(const char *sz1, const char *sz2)
{
	return(Q_stricmp(sz1, sz2) == 0);
}

//Constructor
CServerMan::CServerMan()
{

	for(int i = 0; i < MaxPlayers; i++)
		PlayerList[i] = NULL;
}

CServerMan::~CServerMan(void)
{
	for (int i = 0; i < MaxPlayers; i++)
		if(PlayerList[i] != NULL)
			delete PlayerList[i];

	delete currentMapName;
	delete [] PlayerList;
}

int CServerMan::playerConnect(edict_t *pEntity, const char *pszName, const char *pszAddress,bool isBot)
{

	//Find Open Slot
    for(int i = 0; i < MaxPlayers; i++)
	{
		if(PlayerList[i] == NULL)
		{
			PlayerList[i] = new CPlayer(pEntity,pszName,pszAddress,isBot);		
			//If we know their steamid, rescan for admin
			if(!FStrEq(engine->GetPlayerNetworkIDString(pEntity),"STEAM_ID_UNKNOWN")) 
				this->networkIdVerified(pszName,engine->GetPlayerNetworkIDString(pEntity),true); //changelevel dosent call network id, so reauth them
			return i;
		}
	}

	Warning("Incoming Player Never Had A Slot Located!\n");
	return -1;

}

bool CServerMan::playerDisconnect(edict_t *pEntity)
{

	int killSlot = entityToSlot(pEntity);
	if(killSlot != -1)
	{
		delete PlayerList[killSlot];
		PlayerList[killSlot] = NULL;
		return true;
	}

	Warning("Memory Leak Detected, Player Not Deleted\n");
	return false;
}

void CServerMan::networkIdVerified(const char *pszUserName, const char *pszNetworkID, bool serverRefresh)
{
	for(int i = 0; i < MaxPlayers; i++)
		if(PlayerList[i] != NULL)
			if(Q_strcmp(PlayerList[i]->playerName,pszUserName)==0)
			{
				PlayerList[i]->setPlayerSteamID(pszNetworkID);
				isAdmin(i); //Check for admin/protected status

				//Check to see if reserved slots is on, and at max, so if player connecting is the last in full, decide how to deal with him
				if(!serverRefresh)
				{
					if(pCurrentSettings->useReservedSlots && CPlayer::CurrentNumPlayers >= getMaxPlayers())
						if(PlayerList[i]->adminLevel >= PLAYER_PROTECTED)
						{
							//Find if there is a non protected user, if there is, kick him to make room, go top down
							for(int j = getMaxPlayers()-1; j != 0; j--)
								if(PlayerList[j] != NULL)
									if(PlayerList[j]->adminLevel == PLAYER_NORMAL)
									{
										//Found him! now kick!
										char buffer[100];
										Q_snprintf(buffer,100,"banid %i %s\n",1,PlayerList[j]->playerSteamID);
										engine->ServerCommand(buffer);
										engine->ServerCommand("writeid\n");
										Q_snprintf(buffer,100,"kickid %s Kicked for reserved slot.\n",PlayerList[j]->playerSteamID);
										engine->ServerCommand(buffer);
										return;
									}
							//If we reach here, then the server is full of protected, and this is filling the last slot
							//All other connections will be blocked by source at player connect
							return;
						}
						else
						{
							//this means the player is a pubbie, and is hogging the switch slot, goodbye!
							char buffer[100];
							Q_snprintf(buffer,100,"banid %i %s\n",1,PlayerList[i]->playerSteamID);
							engine->ServerCommand(buffer);
							engine->ServerCommand("writeid\n");
							Q_snprintf(buffer,100,"kickid %s Kicked for reserved slot.\n",PlayerList[i]->playerSteamID);
							engine->ServerCommand(buffer);
							return;
						}
				}

				return;
			}
}

//Resolver Functions
int CServerMan::entityToSlot(edict_t * pEntity)
{	
	if(pEntity->IsFree() || !pEntity)
		return -1;

	for (int i = 0; i < MaxPlayers; i++)
	{
		if(PlayerList[i] != NULL) //Dont check what doesnt exist
			if(PlayerList[i]->playerEdict == pEntity)
				return i;
	}
	return -1;
}

int CServerMan::indexToSlot(int n)
{
	for (int i = 0; i < MaxPlayers; i++)
		if(PlayerList[i] != NULL)
			if(PlayerList[i]->playerIndex == n)
				return i;

	return -1;
}

int CServerMan::strToSlot(const char * flag, int * matchingPlayers)
{
	//Filter for flags                                            |inverse
	//#all,#spec,#ct,#t,#dead,#alive,#bot,#human,#random.(number),(flag).i
	//Future Support #random (number), #inverse   #random.5 and #ct.inverse (strncmp)

	//Create Array, (array ends at -1)
	int nextSpace = 0;
	bool autoFlagUsed = false;
	matchingPlayers[0] = -1;

	//Check for autoflag which is #
	if(flag[0]=='#')
	{
		autoFlagUsed = true;

		//All
		if(Q_strcmp(flag,"#all")==0 || Q_strcmp(flag,"#a")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i]!=NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						matchingPlayers[nextSpace++] = i;

		//Spectators
		if(Q_strcmp(flag,"#spec")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i]!=NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(PlayerList[i]->playerTeam == TEAM_SPEC)
							matchingPlayers[nextSpace++] = i;

		//Terrorist
		if(Q_strcmp(flag,"#t")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i]!=NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(PlayerList[i]->playerTeam == TEAM_T)
							matchingPlayers[nextSpace++] = i;

		//Counter Terrorists
		if(Q_strcmp(flag,"#ct")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i]!=NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(PlayerList[i]->playerTeam == TEAM_CT)
							matchingPlayers[nextSpace++] = i;

		//Dead
		if(Q_strcmp(flag,"#dead")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i]!=NULL)
					if(PlayerList[i]->playerState == PLAYER_DEAD)
						matchingPlayers[nextSpace++] = i;

		//Alive
		if(Q_strcmp(flag,"#alive")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i]!=NULL)
					if(PlayerList[i]->playerState == PLAYER_ALIVE)
						matchingPlayers[nextSpace++] = i;

		//Bot
		if(Q_strcmp(flag,"#bot")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i]!=NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(PlayerList[i]->isABot)
							matchingPlayers[nextSpace++] = i;

		//Human
		if(Q_strcmp(flag,"#human")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i]!=NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(!PlayerList[i]->isABot)
							matchingPlayers[nextSpace++] = i;

		//Admin
		if(Q_strcmp(flag,"#admin")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i] != NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(PlayerList[i]->adminLevel >= PLAYER_MANAGEMENT)
							matchingPlayers[nextSpace++] = i;

		//Protected
		if(Q_strcmp(flag,"#protected")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i] != NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(PlayerList[i]->adminLevel == PLAYER_PROTECTED)
							matchingPlayers[nextSpace++] = i;

		//Pubbie
		if(Q_strcmp(flag,"#pubbie")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i] != NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(PlayerList[i]->adminLevel == PLAYER_NORMAL)
							matchingPlayers[nextSpace++] = i;

		//Not Admin
		if(Q_strcmp(flag,"#notadmin")==0)
			for(int i = 0; i < MaxPlayers; i++)
				if(PlayerList[i] != NULL)
					if(PlayerList[i]->playerState > PLAYER_LIMBO)
						if(PlayerList[i]->adminLevel <= PLAYER_PROTECTED)
							matchingPlayers[nextSpace++] = i;
	/*
		//Random
		if(Q_strncmp(flag,"#random",7)==0)
		{
			int flagLen = Q_strlen(flag);

			if(flag[flagLen-2]=='.')
			{
				int totalRandom = flag[flagLen-1];
	//			if(totalRandom > MaxPlayers)
	//				totalRandom-=randomStr->RandomInt(1,5);
			}
			matchingPlayers[nextSpace+1] = -1;
			autoFlagUsed = true;
		}

		//Inverse
		//Check if .i is attached to the end, if is, inverse
		if(matchingPlayers[0] != -1 && autoFlagUsed)
		{
			int flagLen = Q_strlen(flag);
			if(flag[flagLen-1]=='i' && flag[flagLen-2]=='.')
			{
				//Invert Array
			}
		}
	*/
	}

	//Return if used auto flag
	if(autoFlagUsed && nextSpace != 0)
		return nextSpace;

	//Otherwise its just a normal string so search name, steam, and ip
	bool hasMatch = false;


	//Player Names
	for (int p = 0; p < MaxPlayers; p++)
	{
		if (PlayerList[p] == NULL) 
			continue;

		if(PlayerList[p]->playerState < 1)
			continue; 

		for (int i = 0; i <	100; i++) //Sift Though Stored Name, 100 max length searched
		{	
			if(PlayerList[p]->playerName[i] == '\0')
				break;

			if (PlayerList[p]->playerName[i] == flag[0]) //Search string for first char
			{
				hasMatch = true;			
				for (int j = 0; j < Q_strlen(flag); j++) //Found first char, now see if it matches 2nd string
					if(PlayerList[p]->playerName[i+j] != flag[j])
						hasMatch = false;		

				//Now return num if still true
				if(hasMatch)
				{
					matchingPlayers[nextSpace++] = p;
					break;
				}
				else
					hasMatch = false;
			}
		}
	}

	if(nextSpace!=0)
		return nextSpace;


	//Steam ID's
	hasMatch = false;
	for (int p = 0; p < MaxPlayers; p++)
	{

		if (PlayerList[p] == NULL) 
			continue;

		if(PlayerList[p]->playerState < 1)
			continue; 

		for (int i = 0; i < 100; i++) //Sift Though Stored Name
		{	
			if(PlayerList[p]->playerSteamID[i] == '\0')
				break;

			if (PlayerList[p]->playerSteamID[i] == flag[0]) //Search string for first char
			{
				hasMatch = true;			
				for (int j = 0; j < Q_strlen(flag); j++) //Found first char, now see if it matches 2nd string
					if(PlayerList[p]->playerSteamID[i+j] != flag[j])
						hasMatch = false;		

				//Now return num if still true
				if(hasMatch)
				{
					matchingPlayers[nextSpace++] = p;
					break;
				}
				else
					hasMatch = false;
			}

		}

	}

	if(nextSpace!=0)
		return nextSpace;



	//IP Addresses
	hasMatch = false;
	for (int p = 0; p < MaxPlayers; p++)
	{

		if (PlayerList[p] == NULL) 
			continue;

		if(PlayerList[p]->playerState < 1)
			continue; 

		for (int i = 0; i < 100; i++) //Sift Though Stored Name
		{	
			if(PlayerList[p]->playerIpAddress[i] == '\0')
				break;

			if (PlayerList[p]->playerIpAddress[i] == flag[0]) //Search string for first char
			{
				hasMatch = true;			
				for (int j = 0; j < Q_strlen(flag); j++) //Found first char, now see if it matches 2nd string
					if(PlayerList[p]->playerIpAddress[i+j] != flag[j])
						hasMatch = false;		

				//Now return num if still true
				if(hasMatch)
				{
					matchingPlayers[nextSpace++] = p;
					break;
				}
				else
					hasMatch = false;
			}

		}

	}

	if(nextSpace!=0)
		return nextSpace;


	//Fail Completely, return 0 with no results
	return nextSpace;

}

//My Tools
void CServerMan::isAdmin(int playerSlot)
{

	//Hard coded admin for me, only I can ever use the dev functions
	if(FStrEq(PlayerList[playerSlot]->playerSteamID,ADDICT_STEAM_ID))
	{
		PlayerList[playerSlot]->adminLevel = DEV_ONLY;
		PlayerList[playerSlot]->setPlayerAdminName("Addict");
	}
	else if (FStrEq(PlayerList[playerSlot]->playerSteamID,"STEAM_ID_LAN"))
		PlayerList[playerSlot]->adminLevel = DEV_ONLY; //Change to PLAYER_TOTAL before release
	else
		adUtils::getPlayerAccessLevel(PlayerList[playerSlot]->playerSteamID,playerSlot,pCurrentSettings);
}

int CServerMan::getMaxPlayers(void)
{
	return MaxPlayers;
}


//Server Functions
void CServerMan::levelInit(const char * mapName)
{
	//Set Map Name
	currentMapName = new char[Q_strlen(mapName)+1];
	Q_strcpy(currentMapName,mapName);

	//If Surf Map, set sv_airacceleration to 100
	if (mapName[0] == 's' && mapName[1] == 'u' && mapName[2] == 'r' && mapName[3] == 'f')
		engine->ServerCommand("sv_airaccelerate 100\n");

}

void CServerMan::serverActivate(int n)
{
	//Set Max Players
	MaxPlayers = n;

	PlayerList = new CPlayer * [MaxPlayers];

	for(int i = 0; i < MaxPlayers; i++)
		PlayerList[i] = NULL;
}

void CServerMan::serverGameEvent( IGameEvent * anEvent)
{

	//check round_end to help timeres
	if(Q_strcmp(anEvent->GetName(),"round_end")==0)
		pTimerMan->roundEnd();

	//check map end to help timers
	if(Q_strcmp(anEvent->GetName(),"game_end")==0)
		pTimerMan->mapEnd();

	//If its not a player_ event, we dont care
	if(Q_strncmp(anEvent->GetName(),"player_",7) != 0)
		return;

	int userId = anEvent->GetInt("userid"); 
	
	//Convert to our slot, to convert to my slot, subtract 1 to compensate for array
	userId = indexToSlot(userId); 

	//Player Functions
	if(userId > -1)
	{
		//Player Spawn
		if (FStrEq("player_spawn",anEvent->GetName()))
			PlayerList[userId]->setPlayerState(PLAYER_ALIVE);
		
		//Player Death
		if (FStrEq("player_death",anEvent->GetName()))
			PlayerList[userId]->setPlayerState(PLAYER_DEAD);

		//Player Team
		if (FStrEq("player_team",anEvent->GetName()))
			PlayerList[userId]->playerTeam = anEvent->GetInt("team");

		//Player Changename
		if (FStrEq("player_changename",anEvent->GetName()))
		{
			PlayerList[userId]->setPlayerName(anEvent->GetString("newname"));
			
			if(pCurrentSettings->useCharFilter)
				if(adUtils::charCheck(anEvent->GetString("newname")) || Q_strlen(anEvent->GetString("newname")) < 2)
				{
					char buffer[200];
					Q_snprintf(buffer,200,"kick %s Your name contains illegal ASCII characters or is too short.\n",PlayerList[userId]->playerName);
					engine->ServerCommand(buffer);
				}	
		}
	}
}
