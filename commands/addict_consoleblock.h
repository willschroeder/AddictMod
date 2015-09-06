#ifndef ADDICT_CONSOLEBLOCK_H
#define ADDICT_CONSOLEBLOCK_H

//Do Includes Here
#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "addict_globals.h"

//Include Externs Here
extern CServerMan * pServerMan;
extern IVEngineServer * engine;

class addict_consoleblock : public CCommandBase
{
public:
	addict_consoleblock();
	virtual ~addict_consoleblock();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_consoleblock::addict_consoleblock() : CCommandBase()
{
	setCommandName("ad_consoleblock");
	setCommandUsage("<access flag> <action #check/#kick> <password to set if you want one>");
	setCommandLevel(PLAYER_TOTAL);
}

addict_consoleblock::~addict_consoleblock()
{
	//Deconstructor
}

bool addict_consoleblock::onCommand(int numFlags, char ** flagList, int playerNum)
{
	
	if(numFlags < 1)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	commandStart(&selectedPlayers);

	selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

	//Return if None Found
	if(selectedPlayersCount == 0)
	{
		if(playerNum != -1) engine->ClientCommand(pServerMan->PlayerList[playerNum]->playerEdict,"No clients found matching that string!\n");
		return commandEnd(&selectedPlayers,true);
	}

	char buffer[100];

	if(Q_strcmp("#check",flagList[1])==0)
		for(int i = 0; i < selectedPlayersCount; i++)
			engine->ClientCommand(pServerMan->PlayerList[selectedPlayers[i]]->playerEdict,"ad_usertools #executesconsolecommands");

	if(Q_strcmp("#kick",flagList[1])==0)
		for(int i = 0; i < selectedPlayersCount; i++)
		{													
			//If they fail, kick em
			if(pServerMan->PlayerList[selectedPlayers[i]]->blockConsoleCommands)
			{
				Q_snprintf(buffer,100,"kick %s\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName);
				engine->ServerCommand(buffer);
			}

			//Now set back for future tests
			pServerMan->PlayerList[selectedPlayers[i]]->blockConsoleCommands = true;

			//set password
			if(numFlags > 2)
			{
				Q_snprintf(buffer,100,"sv_password \"%s\"\n",flagList[1]);
				engine->ServerCommand(buffer);
			}
		}

	return commandEnd(&selectedPlayers,true);
}


#endif
