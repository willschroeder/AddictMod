#ifndef ADDICT_MAP_H
#define ADDICT_MAP_H

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

class addict_map : public CCommandBase
{
public:
	addict_map();
	virtual ~addict_map();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_map::addict_map() : CCommandBase()
{
	setCommandName("ad_map");
	setCommandUsage("<map name> Changes the level.");
	setCommandLevel(PLAYER_MANAGEMENT);
}

addict_map::~addict_map()
{
	//Deconstructor
}

bool addict_map::onCommand(int numFlags, char ** flagList, int playerNum)
{
	if(numFlags < 1)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	//Note, not dealing with players, so we dont need to thread command start/end

	if(!engine->IsMapValid(flagList[0]))
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"That map is not valid/dosent exist.\n");
		return true;
	}

	//I dunno what the NULL means at the end, but what ever
	engine->ChangeLevel(flagList[0],NULL);
	return true;

}


#endif
