#ifndef ADDICT_NEXTMAP_H
#define ADDICT_NEXTMAP_H

#include "CModeBase.h"
#include "CPlayer.h"
#include "CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "addict_globals.h"
#include "icvar.h"
#include <string>
#include <fstream>
#include <ctime>
#include <cmath>
using namespace std;


extern CServerMan * pServerMan;
extern IVEngineServer	* engine;
extern IUniformRandomStream * randomStr;
extern CGlobalVars * gpGlobals;
class addict_nextmap : public CModeBase
{

public:

	addict_nextmap();
	~addict_nextmap();

	virtual bool userCommandInterceptor(int, char **, int);	
	virtual bool systemCommandInterceptor(int, char **);
	virtual bool eventParser(IGameEvent *);

private:

	void genNewMap(void);

	char * nextMap;
	int * selectedPlayers;
	int selectedPlayersCount;
	int numMaps;
	bool nextMapManualSet;

};

addict_nextmap::addict_nextmap() : CModeBase()
{
	setModeName("nextmap");
	setModeDesc("<nextmap name/goto> Sets nextmap if you are admin, otherwise just lists it. goto switches to the nextmap.");
	setModeLevel(PLAYER_MANAGEMENT);

	numMaps = 0;
}

addict_nextmap::~addict_nextmap()
{
	if(nextMap != NULL)
		delete nextMap;
}

bool addict_nextmap::userCommandInterceptor(int numFlags, char ** flagList, int playerNum)
{
	if(!pCurrentSettings->useMapRandomizer)
		return false;

	//Set nextmap if admin
	if(numFlags > 0 && playerNum != -1)
	{
		if(Q_strcmp(flagList[0],"goto")==0)
		{
			dp_string temp(1);
			temp.addString(nextMap);
			pCommandMan->commandExecute("ad_map",temp.dpList,temp.getMaxItems());
			return true;
		}

		if(Q_strlen(flagList[0]) > 3) //at least de_ or something, fights off spaces
		{
			if(pServerMan->PlayerList[playerNum]->adminLevel >= PLAYER_MANAGEMENT) //Player Call
			{
				if(engine->IsMapValid(flagList[0]))
				{
					if(nextMap != NULL)
						delete nextMap;

					nextMap = new char[Q_strlen(flagList[0])+1];
					Q_strcpy(nextMap,flagList[0]);
					nextMapManualSet = true;
				}	
			}
		}
	}

	//return true because we dont know the nextmap
	if(nextMap == NULL || numMaps == 0)
		return true;
	
	//Getting here means map was never found, so its the first map
	char buffer[100];
	dp_string temp(2);
	temp.addString("#all");
	temp.addString(nextMap);
	pCommandMan->commandExecute("ad_announce",temp.dpList,temp.getMaxItems());

	Q_snprintf(buffer,100,"say Next Map: %s\n",nextMap);
	engine->ServerCommand(buffer);
	return true;
}

bool addict_nextmap::systemCommandInterceptor(int numFlags, char ** flagList)
{
	if(Q_strcmp(flagList[0],"#mapstart")==0)
	{
		if(pCurrentSettings->useMapRandomizer)
			isActive = true;
		else
			isActive = false;

		if(nextMap != NULL)
			delete nextMap;

		nextMap = NULL;

		nextMapManualSet = false;

		if(numMaps == 0)
		{
			//Get number of maps
			string curLine;

			//Form string to file
			char buffer[100];
			engine->GetGameDir(buffer,100);
			curLine.append(buffer);
			curLine.append("\\maplist.txt");
			fstream mapFile(curLine.c_str());

			if(mapFile.is_open())
				while(!mapFile.eof())
				{
					getline(mapFile,curLine);
					numMaps++;
				}

			mapFile.close();
		}

		genNewMap();
	}

	return true;
}

bool addict_nextmap::eventParser(IGameEvent * anEvent)
{
	if(Q_strcmp(anEvent->GetName(),"round_end")==0 && pCurrentSettings->useMapRandomizer && CTimerMan::getCurrentTime() > 60)
	{
		ConVar * pConVar = cvar->FindVar( "mp_timelimit" ); 
		float timeLeft = ((pConVar->GetFloat() * 60) - CTimerMan::getCurrentTime()); //in seconds

		if(timeLeft < 0 && nextMap != NULL && !nextMapManualSet)
		{
			char buffer[100];
			Q_snprintf(buffer,100,"nextlevel %s\n",nextMap);
			engine->ServerCommand(buffer);

			int newTimer = pTimerMan->createTimer(5,"ad_map");
			pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(1);
			pTimerMan->timerList[newTimer]->cmdToExecute->addString(nextMap);
			pTimerMan->timerList[newTimer]->howToExec = COMMAND_STACK;
		}
	}
	
	return true;
}

void addict_nextmap::genNewMap(void)
{

	srand(time(0));
	int randomNum = 1;

	if(numMaps != 0)
		randomNum = rand()%numMaps;

	//Get number of maps
	string curLine;

	//Form string to file
	char buffer[100];
	engine->GetGameDir(buffer,100);
	curLine.append(buffer);
	curLine.append("\\maplist.txt");

	fstream mapFile(curLine.c_str());

	if(mapFile.is_open())
	{
		for(int i = 0; !mapFile.eof(); i++)
		{
			getline(mapFile,curLine);
			if(i == randomNum)
			{

				//Check to make sure we wont go back to the next map, if it is, jump to the next
				if(Q_strcmp(curLine.c_str(),pServerMan->currentMapName)==0)
					if(randomNum+1<=numMaps)
					{
						randomNum++;
						continue;
					}
					else
					{
						randomNum = 0;
						i = 0;
						continue;
					}

				if(nextMap != NULL)
					delete nextMap;

				nextMap = NULL;
			
				nextMap = new char[curLine.length()+1];
				//curLine.copy(nextMap,curLine.length()+1);
				Q_strcpy(nextMap,curLine.c_str());

				mapFile.close();
				return;
			}
		}
	}

	mapFile.close();
	
	//Something happened, and we cant open the file, let source do whatever
	if(nextMap != NULL)
		delete nextMap;

	nextMap = NULL;

}

#endif

//Below, some old changemap code, dunno why you would need it tho, but it works
/*
string curLine;
fstream mapFile("./cstrike/maplist.txt");
char buffer[100];

const char * curMap = pServerMan->currentMapName;
bool firstMapRecorded = false;
char * firstMap = NULL;

if(mapFile.is_open())
{
while(!mapFile.eof())
{
//Grab first map
getline(mapFile,curLine);
if(!firstMapRecorded)
{
firstMapRecorded = true;
firstMap = new char[Q_strlen(curLine.c_str()) +1];
Q_strcpy(firstMap,curLine.c_str());
}

//See if we have match
if(Q_strcmp(curLine.c_str(),curMap)==0)
{
//found map, the next map in the list is the next map, if its null, then the map is first on the list
getline(mapFile,curLine);
if(curLine.empty())
{
dp_string temp(2);
temp.addString("#all");
temp.addString(firstMap);
pCommandMan->commandExecute("ad_announce",temp.dpList,temp.getMaxItems());

Q_snprintf(buffer,100,"say Next Map: %s\n",firstMap);
engine->ServerCommand(buffer);
delete firstMap;
return true;
}
else
{
dp_string temp(2);
temp.addString("#all");
temp.addString(curLine.c_str());
pCommandMan->commandExecute("ad_announce",temp.dpList,temp.getMaxItems());

Q_snprintf(buffer,100,"say Next Map: %s\n",curLine.c_str());
engine->ServerCommand(buffer);
delete firstMap;
return true;
}
}
}
//Getting here means map was never found, so its the first map
dp_string temp(2);
temp.addString("#all");
temp.addString(firstMap);
pCommandMan->commandExecute("ad_announce",temp.dpList,temp.getMaxItems());

Q_snprintf(buffer,100,"say Next Map: %s\n",firstMap);
engine->ServerCommand(buffer);
delete firstMap;
mapFile.close();
return true;

}
else
engine->ServerCommand("say No mapcycle.txt present to scan.\n");

if(firstMap != NULL)
delete firstMap;
*/