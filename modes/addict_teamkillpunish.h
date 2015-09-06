#ifndef ADDICT_TKPUNISH_H
#define ADDICT_TKPUNISH_H

#include "CModeBase.h"
#include "CPlayer.h"
#include "eiface.h"
#include "edict.h"
#include "addict_globals.h"

extern CServerMan * pServerMan;
extern CCommandMan * pCommandMan;
extern IVEngineServer	* engine;
extern IPlayerInfoManager * playerinfomanager;
extern userOptions * pCurrentSettings;

class addict_tkpunish : public CModeBase
{

public:

	addict_tkpunish();
	~addict_tkpunish();

	virtual bool userCommandInterceptor(int, char **, int);	
	virtual bool systemCommandInterceptor(int, char **);
	virtual bool eventParser(IGameEvent *);

private:
	int * selectedPlayers;
	int selectedPlayersCount;

	int punishLevel;
	const int damageIfLessThan;

};

addict_tkpunish::addict_tkpunish() : damageIfLessThan(33)
{
	setModeName("ad_tkpunish");
	setModeDesc("<Level of TK Punishment: 0 = Off 1 = Instant Kill 2 = Take Health> Punishes team killers.");
	setModeLevel(PLAYER_MANAGEMENT);

	punishLevel = 0;
}

addict_tkpunish::~addict_tkpunish()
{
	
}

bool addict_tkpunish::userCommandInterceptor(int numFlags, char ** flagList, int playerNum)
{

	if(numFlags < 1)
	{	
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}
	
	int setLevel = Q_atoi(flagList[0]);

	switch(setLevel)
	{
	case 0:
		punishLevel = 0;
		isActive = false;
		break;
	case 1:
		punishLevel = 1;
		isActive = true;
		break;
	case 2:
		punishLevel = 2;
		isActive = true;
		break;
	default:
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Not able to set that level. Use 0,1,2. See ad_help for desc.\n");
		isActive = false;
		return true;
	}

	return true;
}

bool addict_tkpunish::systemCommandInterceptor(int numFlags, char ** flagList)
{
	if(Q_strcmp(flagList[0],"#mapstart")==0)
	{
		punishLevel = pCurrentSettings->tkPunish;

		if(punishLevel > 0)
			isActive = true;
		else
			isActive = false;
	}

	return true;
}

bool addict_tkpunish::eventParser(IGameEvent * anEvent)
{
	//Slay on team kill
	if(punishLevel == 1)
	{
		if(Q_strcmp(anEvent->GetName(),"player_death")==0)
		{
			int attacked = pServerMan->indexToSlot(anEvent->GetInt("userid"));
			int killer = pServerMan->indexToSlot(anEvent->GetInt("attacker"));

			//Dont punish suicide
			if(attacked == killer || killer == -1)
				return true;

			if(pServerMan->PlayerList[attacked]->playerTeam == pServerMan->PlayerList[killer]->playerTeam && 
				pServerMan->PlayerList[killer]->adminLevel <= pServerMan->PlayerList[attacked]->adminLevel)
			{
				dp_string temp(1);
				temp.addString(pServerMan->PlayerList[killer]->playerName);
				pCommandMan->commandExecute("ad_slay",temp.dpList,temp.getMaxItems());
			}
		}
		return true;
	}

	//Do damage based on person
	if(punishLevel == 2)
	{
		if(Q_strcmp(anEvent->GetName(),"player_death")==0)
		{
			int attacked = pServerMan->indexToSlot(anEvent->GetInt("userid"));
			int killer = pServerMan->indexToSlot(anEvent->GetInt("attacker"));

			//Dont punish suicide
			if(attacked == killer || killer == -1)
				return true;

			if(pServerMan->PlayerList[attacked]->playerTeam == pServerMan->PlayerList[killer]->playerTeam && 
				pServerMan->PlayerList[killer]->adminLevel <= pServerMan->PlayerList[attacked]->adminLevel)
			{
				dp_string temp(1);
				temp.addString(pServerMan->PlayerList[killer]->playerSteamID);
				pCommandMan->commandExecute("ad_slay",temp.dpList,temp.getMaxItems());
			}
			return true;
		}

		if(Q_strcmp(anEvent->GetName(),"player_hurt")==0)
		{
			int attacked = pServerMan->indexToSlot(anEvent->GetInt("userid"));
			int killer = pServerMan->indexToSlot(anEvent->GetInt("attacker"));

			//Dont punish suicide
			if(attacked == killer || killer == -1)
				return true;

			int attackedHealthLeft = anEvent->GetInt("health");
			int killerHealthLeft = 100;
			char buffer[100];
			
			IPlayerInfo * attackerInfo = playerinfomanager->GetPlayerInfo(pServerMan->PlayerList[killer]->playerEdict);
			killerHealthLeft = attackerInfo->GetHealth();

			if(pServerMan->PlayerList[attacked]->playerTeam == pServerMan->PlayerList[killer]->playerTeam && 
				pServerMan->PlayerList[killer]->adminLevel <= pServerMan->PlayerList[attacked]->adminLevel)
			{
				dp_string temp(2);
				temp.addString(pServerMan->PlayerList[killer]->playerSteamID);	

				if(attackedHealthLeft > killerHealthLeft)
				{
					//Slay Him
					if((killerHealthLeft - damageIfLessThan) <= 0)
					{
						dp_string temp(1);
						temp.addString(pServerMan->PlayerList[killer]->playerSteamID);
						pCommandMan->commandExecute("ad_slay",temp.dpList,temp.getMaxItems());
						return true;
					}
					else
						temp.addString(itoa(damageIfLessThan,buffer,10)); //damage for 33 health
				}
				else
					temp.addString(itoa(attackedHealthLeft,buffer,10));				

				pCommandMan->commandExecute("ad_health",temp.dpList,temp.getMaxItems());			
			}
			return true;
		}
	}
	return true;
}

#endif