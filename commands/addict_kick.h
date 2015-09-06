#ifndef ADDICT_KICK_H
#define ADDICT_KICK_H

#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "addict_globals.h"

//Bring this in to get to the playerlist
extern CServerMan * pServerMan;
extern IVEngineServer * engine;

class addict_kick : public CCommandBase
{
public:
	addict_kick();
	virtual ~addict_kick();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;

};


addict_kick::addict_kick() : CCommandBase()
{
	setCommandName("ad_kick");
	setCommandUsage("<access flag> Kicks a person from the server.");
	setCommandLevel(PLAYER_MANAGEMENT);
}


addict_kick::~addict_kick()
{
	//Nope, nothing
}


bool addict_kick::onCommand(int numFlags, char ** flagList, int playerNum)
{
	if(numFlags < 1)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	commandStart(&selectedPlayers);
	
	selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 
	
	//Return false if no results
	if(selectedPlayersCount == 0)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"No players found matching the string provided.\n");
		return commandEnd(&selectedPlayers,true);
	}

	char buffer[100];
	for(int i = 0; i < selectedPlayersCount; i++)
	{

		//Dont kick bots to avoid desync
		if(pServerMan->PlayerList[selectedPlayers[i]]->isABot)
			continue;

		Q_snprintf(buffer,100,"kick %s\n",pServerMan->PlayerList[selectedPlayers[i]]->playerName);
		engine->ServerCommand(buffer);
	}
	
	return commandEnd(&selectedPlayers,true);
}

#endif