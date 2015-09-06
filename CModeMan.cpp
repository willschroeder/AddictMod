#include "CModeMan.h"
#include "CModeBase.h"
#include "CServerMan.h"

//Begin Mode Include List - Dont forget to update the number of modes
#include "modes/addict_health.h"
#include "modes/addict_vote.h"
#include "modes/addict_wordfilter.h"
#include "modes/addict_nextmap.h"
#include "modes/addict_teamkillpunish.h"
//End Mode Include List

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern CServerMan * pServerMan;

CModeMan::CModeMan() : NumModes(5) //Update this as you add new modes
{
	ModeList = new CModeBase * [NumModes];
	
	//Blank the array
	for(int i = 0; i < NumModes; i++)
		ModeList[i] = NULL;

	//Create New Here
	ModeList[0] = new addict_health();
	ModeList[1] = new addict_vote();
	ModeList[2] = new addict_wordfilter();
	ModeList[3] = new addict_nextmap();
	ModeList[4] = new addict_tkpunish();

}

CModeMan::~CModeMan()
{
	for(int i = 0; i < NumModes; i++)
		if(ModeList[i] != NULL)
			delete ModeList[i];

	delete ModeList;

}

bool CModeMan::userCommandExecute(char * cmd, int numFlags, char ** flags, int playerSlot)
{
	int modeSlot = nameToSlot(cmd);

	if(modeSlot == -1)
		return false;

	if(playerSlot != -1)
		if(pServerMan->PlayerList[playerSlot]->adminLevel < ModeList[modeSlot]->getModeLevel())
			return false;

	return ModeList[modeSlot]->userCommandInterceptor(numFlags,flags,playerSlot);

}

bool CModeMan::systemCommandExecute(char * cmd,int numFlags, char ** flags)
{
	int modeSlot = nameToSlot(cmd);

	if(modeSlot == -1)
		return false;

	return ModeList[modeSlot]->systemCommandInterceptor(numFlags,flags);
}

void CModeMan::eventManager(IGameEvent * someEvent)
{
	for(int i = 0; i < NumModes; i++)
		if(ModeList[i] != NULL)
			if(ModeList[i]->getModeActive())
				ModeList[i]->eventParser(someEvent);
}


int CModeMan::nameToSlot(char * s)
{
	for (int i = 0; i < NumModes; i++)
		if(ModeList[i] != NULL)
			if (Q_strcmp(ModeList[i]->getModeName(),s) == 0)
				return i;

	return -1;
}

int CModeMan::getNumModes()
{
	return NumModes;
}
