#ifndef ADDICT_HEALTH_H
#define ADDICT_HEALTH_H

#include "CModeBase.h"
#include "CPlayer.h"
#include "CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "addict_globals.h"

extern CServerMan * pServerMan;
extern IVEngineServer	* engine;
extern userOptions * pCurrentSettings;

#define GAME_DLL 1 
#include "cbase.h"

class addict_health : public CModeBase
{

public:

	addict_health();
	~addict_health();

	virtual bool userCommandInterceptor(int, char **, int);	
	virtual bool systemCommandInterceptor(int, char **);
	virtual bool eventParser(IGameEvent *);

private:
	
	int * healthArray;
	int * selectedPlayers;
	int selectedPlayersCount;

};

addict_health::addict_health() : CModeBase()
{
	setModeName("ad_health");
	setModeDesc("<flag> <ammount> Sets a player's health to the ammount.");
	setModeLevel(PLAYER_MANAGEMENT);

	isActive = false;
	healthArray = NULL;
}

addict_health::~addict_health()
{
	if(healthArray != NULL)
		delete healthArray;
}

bool addict_health::userCommandInterceptor(int numFlags, char ** flagList, int playerNum)
{
	//Check for CBase Block
	if(!pCurrentSettings->useCBaseFunctions)
		return true;

	//Create new if none
	if(healthArray == NULL)
	{
		healthArray = new int[pServerMan->getMaxPlayers()];

		for(int i = 0; i < pServerMan->getMaxPlayers(); i++)
			healthArray[i] = 0;
	}

	if(numFlags < 2)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	if(Q_atoi(flagList[1]) > 500)
		isActive = true;
	
	modeStart(&selectedPlayers);

	if(Q_strcmp(flagList[0],"#self")==0)
	{
		selectedPlayers[0] = playerNum;
		selectedPlayersCount = 1;
	}
	else
		selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

	//Return false if no results
	if(selectedPlayersCount == 0)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"No players found matching the string provided.\n");
		return modeEnd(&selectedPlayers,true);
	}

	for(int i = 0; i < selectedPlayersCount; i++)
	{
		CBasePlayer * player = dynamic_cast<CBasePlayer *>(pServerMan->PlayerList[selectedPlayers[i]]->playerEdict->GetUnknown());

		if(Q_atoi(flagList[1]) > 500)
		{
			healthArray[selectedPlayers[i]] = Q_atoi(flagList[1]) - 500;
			player->SetHealth(500);
		}	
		else
		{
			healthArray[selectedPlayers[i]] = 0;
			player->SetHealth(Q_atoi(flagList[1]));
		}
	}

	return modeEnd(&selectedPlayers,true);
}

bool addict_health::systemCommandInterceptor(int numFlags, char ** flagList)
{
	modeStart(&selectedPlayers);

	//Write this in if a mode ever needs it

	return modeEnd(&selectedPlayers,true);
}

bool addict_health::eventParser(IGameEvent * anEvent)
{
	//Make sure events are registered in the serverplugin_addict.cpp

	//Check for CBase Block
	if(!pCurrentSettings->useCBaseFunctions)
		return true;

	if(Q_strcmp("player_hurt",anEvent->GetName())==0)
	{

		int attackedNum = pServerMan->indexToSlot(anEvent->GetInt("userid"));

		if(healthArray[attackedNum] != 0)
		{
			CBasePlayer * player = dynamic_cast<CBasePlayer *>(pServerMan->PlayerList[attackedNum]->playerEdict->GetUnknown());

			int curHealth = player->GetHealth();

			if((curHealth + healthArray[attackedNum]) < 500)
			{
				player->SetHealth(curHealth + healthArray[attackedNum]);
				healthArray[attackedNum] = 0;
			}
			else
			{
				player->SetHealth(500);
				healthArray[attackedNum] = healthArray[attackedNum] - anEvent->GetInt("dmg_health");
			}

		}
		return true;
	}

	//And death
	if(Q_strcmp("player_death",anEvent->GetName())==0)
	{
		int attackedNum = pServerMan->indexToSlot(anEvent->GetInt("userid"));
		healthArray[attackedNum] = 0;
		return true;
	}

	//Round end, wipe all health
	if(Q_strcmp("round_end",anEvent->GetName())==0)
	{
		for(int i = 0; i < pServerMan->getMaxPlayers(); i++)
			healthArray[i] = 0;

		return true;
	}

	return true;
}

#endif