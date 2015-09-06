#include "CCommandMan.h"
#include "CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "dp_string_tools.h"
#include "CModeMan.h"

extern CModeMan * pModeMan;
extern IVEngineServer * engine;
extern CServerMan * pServerMan;

//Begin Command Function Includes
#include "commands/addict_test.h"
#include "commands/addict_help.h"
#include "commands/addict_playerinfo.h"
#include "commands/addict_kick.h"
#include "commands/addict_kick_ban.h"
#include "commands/addict_exec.h"
#include "commands/addict_slay.h"
#include "commands/addict_browser.h"
#include "commands/addict_consoleblock.h"
#include "commands/addict_usertools.h"
#include "commands/addict_map.h"
#include "commands/addict_announce.h"
#include "commands/addict_listmaps.h"
#include "commands/addict_say.h"
//End Command Function Includes

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CCommandMan::CCommandMan() : NumCommands(14) //Update this as you add new commands
{
	CommandList = new CCommandBase * [NumCommands];

	for (int i = 0; i < NumCommands; i++)
		CommandList[i] = NULL;

	//Function Adding
	CommandList[0] = new addict_test();
	CommandList[1] = new addict_help();
	CommandList[2] = new addict_playerinfo();
	CommandList[3] = new addict_kick();
	CommandList[4] = new addict_kick_ban();
	CommandList[5] = new addict_exec();
	CommandList[6] = new addict_slay();
	CommandList[7] = new addict_browser();
	CommandList[8] = new addict_consoleblock();
	CommandList[9] = new addict_usertools();
	CommandList[10] = new addict_map();
	CommandList[11] = new addict_announce();
	CommandList[12] = new addict_listmaps();
	CommandList[13] = new addict_say();
}

CCommandMan::~CCommandMan()
{
	for (int i = 0; i < NumCommands; i++)
		if(CommandList[i] != NULL)
			delete CommandList[i];

	delete CommandList;
}

PLUGIN_RESULT CCommandMan::clientCommand(edict_t * pEntity)
{
	char * cmd = engine->Cmd_Argv(0);

	//Formulate flag array
	const int numFlags = engine->Cmd_Argc() -1; //-1 to compensate for cmd
	dp_string flags(numFlags);
	
	if(numFlags > 0)
		for (int i = 0; i < numFlags; i++)
			flags = engine->Cmd_Argv(i+1);

	if(commandExecute(cmd,flags.dpList,numFlags,pServerMan->entityToSlot(pEntity)))
		return PLUGIN_STOP;
	else
		return PLUGIN_CONTINUE;

}

//command, flag array, number of flags in array, player slot(-1 if none given)
bool CCommandMan::commandExecute(char * cmd, char ** flags, int numFlags, int playerSlot)
{

	int commandSlot = nameToSlot(cmd);

	//Command wasnt found, so see if it belongs to a mode, and return the mode results
	if(commandSlot == -1)
		return pModeMan->userCommandExecute(cmd,numFlags,flags,playerSlot);

	//If is admin command and player is not admin, return false if playerSlot is not -1, which means it came from system
	if(playerSlot != -1)
		if(CommandList[commandSlot]->getCommandLevel() > pServerMan->PlayerList[playerSlot]->adminLevel)
			return false;

	//Execute Command
	return CommandList[commandSlot]->onCommand(numFlags,flags,playerSlot);
}

//Finder Functions
int CCommandMan::nameToSlot(char * s)
{
	for (int i = 0; i < NumCommands; i++)
		if(CommandList[i] != NULL)
			if (Q_strcmp(CommandList[i]->getCommandName(),s) == 0)
				return i;

	return -1;
}

int CCommandMan::getNumCommands()
{
	return NumCommands;
}
