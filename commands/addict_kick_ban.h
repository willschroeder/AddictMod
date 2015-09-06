#ifndef ADDICT_KICK_BAN_H
#define ADDICT_KICK_BAN_H

#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "addict_globals.h"

//Bring this in to get to the playerlist
extern CServerMan * pServerMan;
extern IVEngineServer * engine;
extern userOptions * pCurrentSettings;

class addict_kick_ban : public CCommandBase
{
public:
	addict_kick_ban();
	virtual ~addict_kick_ban();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};


addict_kick_ban::addict_kick_ban() : CCommandBase()
{
	setCommandName("ad_ban");
	setCommandUsage("<access flag> <time in minutes, 0 by default> Kicks and bans a player from the server. || Unban a player #unban <steam_id>");
	setCommandLevel(PLAYER_MANAGEMENT);
}

addict_kick_ban::~addict_kick_ban()
{
	//Nope, nothing
}

bool addict_kick_ban::onCommand(int numFlags, char ** flagList, int playerNum)
{

	if(numFlags < 1)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	commandStart(&selectedPlayers);

	char buffer[100];

	//Check for #unban flag
	if(numFlags > 1)
		if(Q_strcmp(flagList[0],"#unban")==0)
		{
			Q_snprintf(buffer,100,"removeid %s\n",flagList[1]);
			engine->ServerCommand(buffer);
			return commandEnd(&selectedPlayers,true);
		}

	//Resume as a ban command
	selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

	//Return false if no results
	if(selectedPlayersCount == 0)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"No players found matching the string provided.\n");
		return commandEnd(&selectedPlayers,true);
	}

	//Check for time, if none given, do 0 for permaban
	int banTime = 0;

	if(numFlags > 1)
		if(Q_strcmp(flagList[1],"#voteban")==0)
		{
			banTime = pCurrentSettings->lengthOfVoteKickBan;
		}
		else
			banTime = Q_atoi(flagList[1]);

	for(int i = 0; i < selectedPlayersCount; i++)
	{

		if(pServerMan->PlayerList[selectedPlayers[i]]->isABot)
			continue;

		//Make sure you cant ban me
		if(Q_strcmp(pServerMan->PlayerList[selectedPlayers[i]]->playerSteamID,ADDICT_STEAM_ID)==0)
			continue;

		Q_snprintf(buffer,100,"banid %i %s kick\n",banTime,pServerMan->PlayerList[selectedPlayers[i]]->playerSteamID);
		engine->ServerCommand(buffer);
		engine->ServerCommand("writeid\n");
	}

	//Pass the return though commandEnd to do cleanup
	return commandEnd(&selectedPlayers,true);
}

#endif