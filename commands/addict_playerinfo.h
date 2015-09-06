#ifndef ADDICT_PLAYERINFO_H
#define ADDICT_PLAYERINFO_H

#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "addict_globals.h"

//Bring this in to get to the playerlist
extern CServerMan * pServerMan;
extern IVEngineServer * engine;

class addict_playerinfo : public CCommandBase
{
public:
	addict_playerinfo();
	virtual ~addict_playerinfo();

	virtual bool onCommand(int, char **, int = -1); 

//private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_playerinfo::addict_playerinfo() : CCommandBase()
{
	setCommandName("ad_playerinfo");
	setCommandUsage("<access flag> Returns a bunch of info on players.");
	setCommandLevel(PLAYER_NORMAL);
}

addict_playerinfo::~addict_playerinfo()
{
	//Nope, nothing
}

bool addict_playerinfo::onCommand(int numFlags, char ** flagList, int playerNum)
{
	//No server calls
	if(playerNum == -1)
		return true;

	//Must come before commands code, it creates vars that are needed for this instance of the command
	commandStart(&selectedPlayers);

	edict_t * commandExecuter = pServerMan->PlayerList[playerNum]->playerEdict;

	//If no flags, look up info on self, then quit
	if(numFlags == 0)
	{
		selectedPlayers[0] = playerNum;
		selectedPlayersCount = 1;
	} 
	else 
	{ 
		//Otherwise, populate selectedPlayers array with players matching first flag
		selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 
		//Return false if no results
		if(selectedPlayersCount == 0)
		{
			engine->ClientPrintf(commandExecuter,"No players found matching the string provided.\n");
			return commandEnd(&selectedPlayers,true); //We did something, even if not done, return true, otherwise CS says it dosent know the command
		}
	}


	//Temp Vars
	char buffer[20];

	for(int i = 0; i < selectedPlayersCount; i++)
	{
		int lookUp = selectedPlayers[i];

		engine->ClientPrintf(commandExecuter,"\n\n=======================================================================\n");
		engine->ClientPrintf(commandExecuter,"Here is the information AddictMod has stored on the requested person.\n");

		engine->ClientPrintf(commandExecuter,"AddictMod's Slot Num\n");
		engine->ClientPrintf(commandExecuter,itoa(lookUp,buffer,10));

		engine->ClientPrintf(commandExecuter,"\nIndex of Entity/Slot Number\n");
		engine->ClientPrintf(commandExecuter,itoa(pServerMan->PlayerList[lookUp]->playerIndex,buffer,10));

		engine->ClientPrintf(commandExecuter,"\nIP Address/Network ID??\n");
		engine->ClientPrintf(commandExecuter,pServerMan->PlayerList[lookUp]->playerIpAddress);

		engine->ClientPrintf(commandExecuter,"\nPlayer Name\n");
		engine->ClientPrintf(commandExecuter,pServerMan->PlayerList[lookUp]->playerName);

		engine->ClientPrintf(commandExecuter,"\nPlayer's Registered Name\n");
		engine->ClientPrintf(commandExecuter,pServerMan->PlayerList[lookUp]->playerAdminName);

		engine->ClientPrintf(commandExecuter,"\nSteam ID\n");
		engine->ClientPrintf(commandExecuter,pServerMan->PlayerList[lookUp]->playerSteamID);

		engine->ClientPrintf(commandExecuter,"\nTeam\n");
		engine->ClientPrintf(commandExecuter,itoa(pServerMan->PlayerList[lookUp]->playerTeam,buffer,10));

		engine->ClientPrintf(commandExecuter,"\nPlayer Admin Level\n");
		engine->ClientPrintf(commandExecuter,itoa(pServerMan->PlayerList[lookUp]->adminLevel,buffer,10));

		engine->ClientPrintf(commandExecuter,"\nPlayer State\n");
		engine->ClientPrintf(commandExecuter,itoa(pServerMan->PlayerList[lookUp]->playerState,buffer,10));

		engine->ClientPrintf(commandExecuter,"\nThe MaxNum Players is\n");
		engine->ClientPrintf(commandExecuter,itoa(pServerMan->getMaxPlayers(),buffer,10));

		engine->ClientPrintf(commandExecuter,"\nThe total players registered with the plugin is\n");
		engine->ClientPrintf(commandExecuter,itoa(CPlayer::CurrentNumPlayers,buffer,10));

		engine->ClientPrintf(commandExecuter,"\n=======================================================================\n\n");		
	}

	//Pass the return though commandEnd to do cleanup
	return commandEnd(&selectedPlayers,true);
}


#endif