#ifndef ADDICT_SAY_H
#define ADDICT_SAY_H

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

class addict_say : public CCommandBase
{
public:
	addict_say();
	virtual ~addict_say();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_say::addict_say() : CCommandBase()
{
	setCommandName("ad_say");
	setCommandUsage("<Text to say> Says the message using the console so people know its an admin saying it.");
	setCommandLevel(PLAYER_MANAGEMENT);
}

addict_say::~addict_say()
{
	//Deconstructor
}

bool addict_say::onCommand(int numFlags, char ** flagList, int playerNum)
{
	if(numFlags < 1)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	char buffer[300];
	Q_snprintf(buffer,300,"say %s\n",flagList[0]);
	engine->ServerCommand(buffer);

	return true;
}


#endif
