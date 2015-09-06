#ifndef ADDICT_SLAY_H
#define ADDICT_SLAY_H

//Do Includes Here
#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "takedamageinfo.h"
#include "addict_globals.h"

#define GAME_DLL 1 
#include "cbase.h"
#include "takedamageinfo.h"

//Include Externs Here
extern CServerMan * pServerMan;
extern IVEngineServer * engine;
extern CModeMan * pModeMan;

class addict_slay : public CCommandBase
{
public:
	addict_slay();
	virtual ~addict_slay();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_slay::addict_slay() : CCommandBase()
{
	setCommandName("ad_slay");
	setCommandUsage("<access flag> Kills the selected dead! No fancy fireworks, just death.");
	setCommandLevel(PLAYER_MANAGEMENT);
}

addict_slay::~addict_slay()
{
	//Deconstructor
}

bool addict_slay::onCommand(int numFlags, char ** flagList, int playerNum)
{
	//Check for CBase Block
	if(!pCurrentSettings->useCBaseFunctions)
		return true;

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

	//Blank any health they have in ad_health
	dp_string temp(2);
	temp.addString(flagList[0]);
	temp.addString("2");
	pCommandMan->commandExecute("ad_health",temp.dpList,temp.getMaxItems());

	for(int i = 0; i < selectedPlayersCount; i++)
	{
		if(pServerMan->PlayerList[selectedPlayers[i]]->playerState == PLAYER_ALIVE) // Is alive, slaying the dead is bad
		{
			CBaseEntity * toKill = pServerMan->PlayerList[selectedPlayers[i]]->playerEdict->GetUnknown()->GetBaseEntity();
			toKill->SetHealth(1);
			toKill->OnTakeDamage(CTakeDamageInfo(toKill,toKill,9997.0,DMG_GENERIC,0));
			pServerMan->PlayerList[selectedPlayers[i]]->playerState = PLAYER_DEAD;
		}

	}
	return commandEnd(&selectedPlayers,true);
}


#endif
