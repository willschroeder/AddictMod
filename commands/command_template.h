#ifndef ADDICT_TEMPLATE_H
#define ADDICT_TEMPLATE_H

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
	setCommandName("ad_example");
	setCommandUsage("Enter Instructions Here");
	setCommandLevel(DEV_ONLY);
}

addict_help::~addict_help()
{
	//Deconstructor
}

bool addict_help::onCommand(int numFlags, char ** flagList, int playerNum)
{
	if(numFlags < 1)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	//Must come before commands code, it creates vars that are needed for this instance of the command
	commandStart(&selectedPlayers);

	//This will populate already created array with a list of players matching string, ends array with -1 to signal end
	selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

	//Return if None Found
	if(selectedPlayersCount == 0)
	{
		if(playerNum != -1) engine->ClientCommand(pServerMan->PlayerList[playerNum]->playerEdict,"No clients found matching that string!\n");
		return commandEnd(&selectedPlayers,true);
	}

	//Pass the return though commandEnd to do cleanup
	return commandEnd(&selectedPlayers,true);
}


#endif
