#ifndef ADDICT_USERTOOLS_H
#define ADDICT_USERTOOLS_H

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

class addict_usertools : public CCommandBase
{
public:
	addict_usertools();
	virtual ~addict_usertools();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_usertools::addict_usertools() : CCommandBase()
{
	setCommandName("ad_usertools");
	setCommandUsage("Some tools that help you get along with the admin, because they are jerks.");
	setCommandLevel(PLAYER_NORMAL);
}

addict_usertools::~addict_usertools()
{
	//Deconstructor
}

bool addict_usertools::onCommand(int numFlags, char ** flagList, int playerNum)
{
	if(numFlags < 1)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	if(playerNum == -1)
		return true;

	commandStart(&selectedPlayers);
	selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

	if(Q_strcmp(flagList[0],"#blank")==0 && pServerMan->PlayerList[playerNum]->adminLevel >= PLAYER_PROTECTED)
	{

		KeyValues * panel = new KeyValues("", "msg", "about:blank", "title", "Addict Mod Browser");
		panel->SetInt("type", TYPE_URL);
		CBasePlayer *player = dynamic_cast<CBasePlayer *>(pServerMan->PlayerList[playerNum]->playerEdict->GetUnknown());
		player->ShowViewPortPanel("info", false, panel);
		panel->deleteThis();
		return commandEnd(&selectedPlayers,true);
	}

	if(Q_strcmp(flagList[0],"#executesconsolecommands")==0)
	{
		pServerMan->PlayerList[playerNum]->blockConsoleCommands = false;
		Warning("Player %s has executed console check sucessfully.\n",pServerMan->PlayerList[playerNum]->playerName);
		return commandEnd(&selectedPlayers,true);
	}


	return commandEnd(&selectedPlayers,true);
}


#endif
