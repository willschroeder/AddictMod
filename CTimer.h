#ifndef CTIMER_H
#define CTIMER_H

#include "CModeBase.h"
#include "eiface.h"
#include "edict.h"

#include "dp_string_tools.h"

enum RUN_WHERE{COMMAND_STACK,MODE_STACK,EVENT_STACK};
enum DIE_TIMES{RUN_ONCE,ROUND_ENDS,MAP_ENDS};

#define MAX_TIMERS 20 //I know, this should be a vector, we will get them next time Joker!

struct STimer 
{
	STimer()
	{
		commandName = NULL;
		cmdToExecute = NULL;
		eventToFake = NULL;
		runTimes = 1;
	}
	~STimer() 
	{ 
		if(commandName != NULL) 
			delete commandName; 

		if(cmdToExecute != NULL)
			delete cmdToExecute;
	}

	void setCommandName(char * txt)
	{
		commandName = new char[Q_strlen(txt) +1];
		Q_strcpy(commandName,txt);
	}

	float activateTime; //Var thats checked to see if >= time to execute
	int dieWhen; //When the timer is going to be deleted, set with a val from DIE_TIMES
	float interval; //If run more than once, add this to activateTime
	int runTimesTotal; //A Counter if you want to run a limited ammount of times
	int runTimes;
	
	int howToExec; //Where to execute statments, set using RUN_WHERE
	char * commandName;
	dp_string * cmdToExecute; //Double pointer if you want to use a command
	IGameEvent * eventToFake; //Sends an event to the script, keep in mind the event wont be forced by the server, its only for tricking Modes
};

class CTimerMan 
{
public:
	CTimerMan();
	~CTimerMan();

	//Creation/Deletion
	int createTimer(float, char * = NULL);
	int createTimer(float,int,float,int, char * = NULL);
	void destroyTimer(int);

	//Events
	void gameFrame(float);
	void roundEnd(void);
	void timerExecute(int);
	void mapEnd(void);

	
	//TimerList
	STimer * timerList[MAX_TIMERS];
	
	static float getCurrentTime();

private:

	int findOpenSpot();
	int numTimers;
	
};

#endif