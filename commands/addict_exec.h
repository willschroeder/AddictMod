#ifndef ADDICT_EXEC_H
#define ADDICT_EXEC_H

//Do Includes Here
#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "CTimer.h"
#include "addict_globals.h"

//Include Externs Here
extern CServerMan * pServerMan;
extern IVEngineServer * engine;
extern CTimerMan * pTimerMan;

class addict_exec : public CCommandBase
{
public:
	addict_exec();
	virtual ~addict_exec();

	virtual bool onCommand(int, char **, int = -1); 
	void setCheatsOffTimer();

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_exec::addict_exec() : CCommandBase()
{
	setCommandName("ad_exec");
	setCommandUsage("<access flag, #server executes on server> <command> <needs cheats true/false> Executes stuff on clients/server.");
	setCommandLevel(PLAYER_TOTAL);
}

addict_exec::~addict_exec()
{
	//Deconstructor
}

//This command is used if done from player console
bool addict_exec::onCommand(int numFlags, char ** flagList, int playerNum)
{

	if(numFlags < 2)
	{	
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	commandStart(&selectedPlayers);

	//Check to see if we need cheats
	bool needsCheats = false;
	if(numFlags > 2)
		if(Q_strcmp(flagList[2],"true")==0)
			needsCheats = true;


	char buffer[100];
	if(Q_strcmp(flagList[0],"#server")==0)
	{
		if(needsCheats) { engine->ServerCommand("sv_cheats 1\n"); }

		Q_snprintf(buffer,100,"%s\n",flagList[1]);
		engine->ServerCommand(buffer);

		if(needsCheats) { setCheatsOffTimer(); }
		return commandEnd(&selectedPlayers,true);
	}
	else
	{
		if(Q_strcmp(flagList[0],"#self")==0)
		{
			selectedPlayers[0] = playerNum;
			selectedPlayersCount = 1;
		}
		else
			selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

		//Return if None Found
		if(selectedPlayersCount == 0)
		{
			if(playerNum != -1) engine->ClientCommand(pServerMan->PlayerList[playerNum]->playerEdict,"No clients found matching that string!\n");
			return commandEnd(&selectedPlayers,true);
		}

		Q_snprintf(buffer,100,"%s\n",flagList[1]);

		if(needsCheats) { engine->ServerCommand("sv_cheats 1\n"); }

		for(int i = 0; i < selectedPlayersCount; i++)
		{

			//Dont kick bots to avoid desync
			if(pServerMan->PlayerList[selectedPlayers[i]]->isABot)
				continue;

			engine->ClientCommand(pServerMan->PlayerList[selectedPlayers[i]]->playerEdict,buffer);
		}
		if(needsCheats) { setCheatsOffTimer(); }
	}

	//Pass the return though commandEnd to do cleanup
	return commandEnd(&selectedPlayers,true);
}

void addict_exec::setCheatsOffTimer()
{
	int newTimer = pTimerMan->createTimer(.5,"ad_exec");
	pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(2);
	pTimerMan->timerList[newTimer]->cmdToExecute->addString("#server");
	pTimerMan->timerList[newTimer]->cmdToExecute->addString("sv_cheats 0");
}

#endif
