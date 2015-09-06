#ifndef ADDICT_LISTMAPS_H
#define ADDICT_LISTMAPS_H

//Do Includes Here
#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "addict_globals.h"
#include <string>
#include <fstream>

using namespace std;

//Include Externs Here
extern CServerMan * pServerMan;
extern IVEngineServer * engine;

class addict_listmaps : public CCommandBase
{
public:
	addict_listmaps();
	virtual ~addict_listmaps();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

addict_listmaps::addict_listmaps() : CCommandBase()
{
	setCommandName("listmaps");
	setCommandUsage("<none> Lists all maps in maplist.txt");
	setCommandLevel(PLAYER_NORMAL);
}

addict_listmaps::~addict_listmaps()
{
	//Deconstructor
}

bool addict_listmaps::onCommand(int numFlags, char ** flagList, int playerNum)
{
	string curLine;

	//Form string to file
	char buffer[100];
	engine->GetGameDir(buffer,100);
	curLine.append(buffer);
	curLine.append("\\maplist.txt");

	fstream mapFile(curLine.c_str());

	engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"============Begin Map List============\n");
	if(mapFile.is_open())
	{
		while(!mapFile.eof())
		{
			getline(mapFile,curLine);
			Q_snprintf(buffer,100,"%s \n",curLine.c_str());
			engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,buffer);
		}
	}
	else
		engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Error: Unable to open maplist!\n");

	engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"============End Map List============\n");

	mapFile.close();

	return true;
}


#endif
