#ifndef ADDICT_VOTE_H
#define ADDICT_VOTE_H

#include "CModeBase.h"
#include "CPlayer.h"
#include "eiface.h"
#include "edict.h"
#include "addict_globals.h"

extern CServerMan * pServerMan;
extern IVEngineServer	* engine;

enum PLAYER_VOTE_STATUS{CANT_VOTE,HASNT_VOTED,VOTED_NO,VOTED_YES};

class addict_vote : public CModeBase
{

public:

	addict_vote();
	~addict_vote();

	virtual bool userCommandInterceptor(int, char **, int);	
	virtual bool systemCommandInterceptor(int, char **);
	virtual bool eventParser(IGameEvent *);

private:
	int * selectedPlayers;
	int selectedPlayersCount;

	int * voteList;

};

addict_vote::addict_vote()
{
	setModeName("ad_health");
	setModeDesc("<map,slay,kick> <name>");
	setModeLevel(PLAYER_MANAGEMENT);

	isActive = false;
	voteList = NULL;
}

addict_vote::~addict_vote()
{
	if(voteList != NULL)
		delete voteList;
}

bool addict_vote::userCommandInterceptor(int numFlags, char ** flagList, int playerNum)
{
	if(numFlags < 2)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	modeStart(&selectedPlayers);

	selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

	//Return false if no results
	if(selectedPlayersCount == 0)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"No players found matching the string provided.\n");
		return modeEnd(&selectedPlayers,true);
	}

	return modeEnd(&selectedPlayers,true);
}

bool addict_vote::systemCommandInterceptor(int numFlags, char ** flagList)
{
	modeStart(&selectedPlayers);

	return modeEnd(&selectedPlayers,true);
}

bool addict_vote::eventParser(IGameEvent * anEvent)
{
	return true;
}

#endif