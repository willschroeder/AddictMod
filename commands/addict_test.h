/*
Todo:

Figure out how to resolve to CBasePlayer (animating, and others?)
How do i get parts of a player, the entities that make it up
Does canceling events cancel it or not, can i change anything?

*/

#ifndef ADDICT_TEST_H
#define ADDICT_TEST_H

//Do Includes Here
#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "CTimer.h"
#include "addict_globals.h"

//Valve
#include "igameevents.h"



#define GAME_DLL 1 
#include "cbase.h"
#include "player.h"
#include "baseplayer_shared.h"
#include "takedamageinfo.h"
//#include "recipientfilter.h" //messages
#include "irecipientfilter.h"
#include "bitbuf.h" //messages

/*
#include "basecombatcharacter.h"
*/

// VALVE: memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//Include Externs Here
extern CServerMan * pServerMan;
extern IVEngineServer * engine;
extern IGameEventManager2 * gameeventmanager;
extern CTimerMan * pTimerMan;

class addict_test : public CCommandBase
{
public:
	addict_test();
	virtual ~addict_test();

	virtual bool onCommand(int, char **, int = -1); 

	//My Temp Vars
	edict_t * temp;
	bool backOn;

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_test::addict_test() : CCommandBase()
{
	setCommandName("ad_test");
	setCommandUsage("DO NOT use this command, there is test code in here that may total the server.");
	setCommandLevel(DEV_ONLY);

	temp = new edict_t;
	backOn = false;
}

addict_test::~addict_test()
{
	//Deconstructor
	delete temp;
}

bool addict_test::onCommand(int numFlags, char ** flagList, int playerNum)
{
	//Must come before commands code, it creates vars that are needed for this instance of the command
	commandStart(&selectedPlayers);

	/*
		Notes, on command creation, create a struct that holds info for each person on the server if they bind a player, so you can bind perma keys when used
		Or seperate view and controls, and just keep an array that matches the player executing to a player control int
	*/

	if(Q_strcmp(flagList[0],"#self")==0)
	{
		/*
		CRecipientFilter filter;
		filter.AddAllPlayers();

		bf_write * message = engine->UserMessageBegin((IRecipientFilter *)&filter,3);
		message->WriteString("Message to all, Hello");
		engine->MessageEnd();

		
		http://www.sourceplugins.com/viewtopic.php?t=3810
		http://www.hl2coding.com/forums/viewtopic.php?t=1112&highlight=bfwrite
		*/
		CBasePlayer * player = dynamic_cast<CBasePlayer *>(pServerMan->PlayerList[playerNum]->playerEdict->GetUnknown());
		player->CommitSuicide();
	}

	if(Q_strcmp(flagList[0],"#setview")==0)
	{
		int dudeInt = atoi(flagList[1]);
		if(pServerMan->PlayerList[dudeInt] != NULL)
		{
			Warning("Should set view to player %i\n",dudeInt);
			temp = pServerMan->PlayerList[playerNum]->playerEdict;                              
			engine->SetView(pServerMan->PlayerList[playerNum]->playerEdict,pServerMan->PlayerList[dudeInt]->playerEdict);
		}
	}

	if(Q_strcmp(flagList[0],"#backward")==0)
	{
		int dudeInt = atoi(flagList[1]);
		if(pServerMan->PlayerList[dudeInt] != NULL)
		{
			if(!backOn)
			{
				engine->ClientCommand(pServerMan->PlayerList[dudeInt]->playerEdict,"+back");
				backOn = true;
			}
			else
			{
				engine->ClientCommand(pServerMan->PlayerList[dudeInt]->playerEdict,"-back");
				backOn = false;
			}

		}
	}


//combine_helicopter
	if(Q_strcmp(flagList[0],"#time")==0)
	{
		extern CGlobalVars *gpGlobals;
		Warning("The server time is: %f\n", gpGlobals->curtime);
	}

	if(Q_strcmp(flagList[0],"#team")==0)
	{
		pServerMan->PlayerList[playerNum]->playerTeam = Q_atoi(flagList[1]);
	}

	if(Q_strcmp(flagList[0],"#hidden")==0)
	{
		//http://www.sourceplugins.com/viewtopic.php?t=3855
		CBasePlayer * test = dynamic_cast<CBasePlayer *>(pServerMan->PlayerList[0]->playerEdict->GetUnknown());
//		test->SetRenderMode(kRenderTransColor);
//		test->SetRenderColorA(100); 
//		test->SetGravity(1.0);
	}

	if(Q_strcmp(flagList[0],"#timer")==0)
	{
		/*
		int newTimer = pTimerMan->createTimer(5.0,"ad_test");
		pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(1);
		pTimerMan->timerList[newTimer]->cmdToExecute->addString("#message");
		*/
		int newTimer = pTimerMan->createTimer(2.0,"ad_exec");
		pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(2);
		pTimerMan->timerList[newTimer]->cmdToExecute->addString("#server");
		pTimerMan->timerList[newTimer]->cmdToExecute->addString("sv_cheats 0");
	}

	if(Q_strcmp(flagList[0],"#message")==0)
	{
		CBasePlayer * test = dynamic_cast<CBasePlayer *>(pServerMan->PlayerList[0]->playerEdict->GetUnknown());
		//test->CanHearChatFrom(pServerMan->PlayerList[0]->playerEdict->GetUnknown());
	}

	if(Q_strcmp(flagList[0],"#spawn")==0)
	{
		CBaseEntity * toKill = pServerMan->PlayerList[0]->playerEdict->GetUnknown()->GetBaseEntity();
		toKill->SetHealth(1);
		toKill->OnTakeDamage(CTakeDamageInfo(toKill,toKill,9997.0,DMG_GENERIC,0));
	}

	if(Q_strcmp(flagList[0],"#msg")==0)
	{
		extern IServerPluginHelpers * helpers;
		extern IServerPluginCallbacks * pIServerPluginCallBacks;

		KeyValues *kv = new KeyValues( "msg" );
		kv->SetString( "title", flagList[1] );
		kv->SetString( "msg", flagList[1] );
		kv->SetColor( "color", Color( 228, 44, 44, 255 ));
		kv->SetInt( "level", 5);
		kv->SetInt( "time", 100);
		helpers->CreateMessage( pServerMan->PlayerList[0]->playerEdict, DIALOG_MSG, kv, pIServerPluginCallBacks );
		kv->deleteThis();
	}

	//Pass the return though commandEnd to do cleanup
	return commandEnd(&selectedPlayers,true);
}



#endif
