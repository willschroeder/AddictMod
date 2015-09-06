#ifndef ADDICT_ANNOUNCE_H
#define ADDICT_ANNOUNCE_H

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
extern IServerPluginHelpers * helpers;
extern IServerPluginCallbacks * pIServerPluginCallBacks;

class addict_announce : public CCommandBase
{
public:
	addict_announce();
	virtual ~addict_announce();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_announce::addict_announce() : CCommandBase()
{
	setCommandName("ad_announce");
	setCommandUsage("<flag> <message in quotes> Puts message in red text on screen. Limited chars.");
	setCommandLevel(PLAYER_MANAGEMENT);
}

addict_announce::~addict_announce()
{
	//Deconstructor
}

bool addict_announce::onCommand(int numFlags, char ** flagList, int playerNum)
{
	if(numFlags < 2)
	{
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	commandStart(&selectedPlayers);

	selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

	if(selectedPlayersCount == 0)
	{
		if(playerNum != -1) engine->ClientCommand(pServerMan->PlayerList[playerNum]->playerEdict,"No clients found matching that string!\n");
		return commandEnd(&selectedPlayers,true);
	}

	KeyValues *kv = new KeyValues( "msg" );
	kv->SetString( "title", flagList[1] );
	kv->SetString( "msg", flagList[1] );
	kv->SetColor( "color", Color( 228, 44, 44, 255 ));
	kv->SetInt( "level", 5);
	kv->SetInt( "time", 100);

	for(int i = 0; i < selectedPlayersCount; i++)
	{
		if(pServerMan->PlayerList[selectedPlayers[i]]->isABot)
			continue;

		helpers->CreateMessage( pServerMan->PlayerList[selectedPlayers[i]]->playerEdict, DIALOG_MSG, kv, pIServerPluginCallBacks );
	}

	kv->deleteThis();


	return commandEnd(&selectedPlayers,true);
}


#endif
