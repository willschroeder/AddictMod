#ifndef ADDICT_HELP_H
#define ADDICT_HELP_H

//Do Includes Here
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "../CModeBase.h"
#include "addict_globals.h"

//Include Externs Here
extern IVEngineServer * engine;
extern CCommandMan * pCommandMan;
extern CServerMan * pServerMan;
extern CModeMan * pModeMan;

class addict_help : public CCommandBase
{
public:
	addict_help();
	virtual ~addict_help();

	virtual bool onCommand(int, char **, int = -1); 


private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_help::addict_help() : CCommandBase()
{
	setCommandName("ad_help");
	setCommandUsage("(No Flags) Lists all the commands available to you.");
	setCommandLevel(PLAYER_NORMAL);
}

addict_help::~addict_help()
{
	//Deconstructor
}


bool addict_help::onCommand(int numFlags, char ** flagList, int playerNum)
{
	//Protect from server call, why would a server call this?
	if(playerNum == -1)
		return true;

	commandStart(&selectedPlayers);

	char buffer[20];
	int onNum = 0;

	for(int i = 0; i < pCommandMan->getNumCommands(); i++)
	{
		onNum++;

		if(pCommandMan->CommandList[i] == NULL)
			continue;

		if(pCommandMan->CommandList[i]->getCommandLevel() <= pServerMan->PlayerList[playerNum]->adminLevel) //if its an admin command
		{

				engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,itoa(i,buffer,10));
				engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,") ");
				engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,pCommandMan->CommandList[i]->getCommandName());
				engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict," - ");
				engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,pCommandMan->CommandList[i]->getCommandUsage());
				engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"\n");
				continue;

		} 
	}

	//Now List Modes
	for(int j = 0; j < pModeMan->getNumModes(); j++)
	{
		if(pModeMan->ModeList[j] == NULL)
			continue;

		if(pModeMan->ModeList[j]->getModeLevel() <= pServerMan->PlayerList[playerNum]->adminLevel) //if its an admin command
		{
			engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,itoa(j + onNum,buffer,10));
			engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,") ");
			engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,pModeMan->ModeList[j]->getModeName());
			engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict," - ");
			engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,pModeMan->ModeList[j]->getModeDesc());
			engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"\n");
		}
	}
	

	//Pass the return though commandEnd to do cleanup
	return commandEnd(&selectedPlayers,true);
}


#endif