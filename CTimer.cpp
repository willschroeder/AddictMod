#include "CModeMan.h"
#include "CCommandMan.h"
#include "CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "CTimer.h"
#include "dp_string_tools.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


extern CGlobalVars * gpGlobals;
extern CServerMan * pServerMan;
extern CCommandMan * pCommandMan;
extern CModeMan * pModeMan;

CTimerMan::CTimerMan()
{
	numTimers = 0;

	for(int i = 0; i < MAX_TIMERS; i++)
		timerList[i] = NULL;
}

CTimerMan::~CTimerMan()
{
	for(int i = 0; i < MAX_TIMERS; i++)
		if(timerList[i] != NULL)
			delete timerList[i];
}

//Creation/Deletion
int CTimerMan::createTimer(float actT, char * txt)
{
	int newSlot = findOpenSpot();
	timerList[newSlot] = new STimer();

	timerList[newSlot]->activateTime = getCurrentTime() + actT;
	timerList[newSlot]->interval = -1;
	timerList[newSlot]->runTimesTotal = -1;
	timerList[newSlot]->dieWhen = RUN_ONCE;

	if(txt != NULL)
	{
		timerList[newSlot]->setCommandName(txt);
		timerList[newSlot]->howToExec = COMMAND_STACK;
	}

	return newSlot;
}

int CTimerMan::createTimer(float actT,int dieW, float intraV, int runT, char * txt)
{
	int newSlot = findOpenSpot();
	timerList[newSlot] = new STimer();

	timerList[newSlot]->activateTime = getCurrentTime() + actT;
	timerList[newSlot]->interval = intraV;
	timerList[newSlot]->runTimesTotal = runT;
	timerList[newSlot]->dieWhen = dieW;

	if(txt != NULL)
		timerList[newSlot]->setCommandName(txt);

	return newSlot;
}

void CTimerMan::destroyTimer(int toKill)
{
	if(timerList[toKill] != NULL)
		if(toKill != -1)
		{
			delete timerList[toKill];
			timerList[toKill] = NULL;
		}
}

//Events
void CTimerMan::gameFrame(float curTime)
{
//	float curTime = getCurrentTime();

	for(int i = 0; i < MAX_TIMERS; i++)
		if(timerList[i] != NULL)
			if(timerList[i]->activateTime <= gpGlobals->curtime)
				timerExecute(i); 
}

void CTimerMan::roundEnd(void)
{
	for(int i = 0; i < MAX_TIMERS; i++)
		if(timerList[i] != NULL)
			if(timerList[i]->dieWhen == ROUND_ENDS)
				delete timerList[i];
}

void CTimerMan::mapEnd(void)
{
	for(int i = 0; i < MAX_TIMERS; i++)
		if(timerList[i] != NULL)
			delete timerList[i];
}

void CTimerMan::timerExecute(int toExe)
{
	//Execute what we need to
	switch (timerList[toExe]->howToExec)
	{
	case COMMAND_STACK:
		if(timerList[toExe]->cmdToExecute != NULL)
			pCommandMan->commandExecute(timerList[toExe]->commandName,timerList[toExe]->cmdToExecute->dpList,timerList[toExe]->cmdToExecute->getMaxItems(),-1); //-1 to exec on sever	

	case MODE_STACK:
		if(timerList[toExe]->cmdToExecute != NULL)
			pModeMan->systemCommandExecute(timerList[toExe]->commandName,timerList[toExe]->cmdToExecute->getMaxItems(),timerList[toExe]->cmdToExecute->dpList);

	case EVENT_STACK:
		if(timerList[toExe]->eventToFake != NULL)
			pModeMan->eventManager(timerList[toExe]->eventToFake);
	}

	//Now handle what happens next

	//If run once, kill it
	if(timerList[toExe]->dieWhen == RUN_ONCE)
	{
		destroyTimer(toExe);
		return;
	}

	//If not dead, advance to slot
	timerList[toExe]->runTimes++;

	//If time up, destroy next run, otherwise, advance
	if(timerList[toExe]->runTimes == timerList[toExe]->runTimesTotal)
		destroyTimer(toExe);
	else
		timerList[toExe]->activateTime = getCurrentTime() + timerList[toExe]->interval;
	
}

//Static
float CTimerMan::getCurrentTime()
{
	return gpGlobals->curtime;
}

//Private
int CTimerMan::findOpenSpot()
{
	for(int i = 0; i < MAX_TIMERS; i++)
		if(timerList[i] == NULL)
			return i;

	return -1;
}

