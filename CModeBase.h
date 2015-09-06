#ifndef CMODEBASE_H
#define CMODEBASE_H

#include "eiface.h"
#include "edict.h"
#include "igameevents.h"

class CModeBase 
{
public:

	CModeBase();
	virtual ~CModeBase();

	char * getModeName();
	char * getModeDesc();
	bool getModeActive();
	int getModeLevel();

	virtual bool systemCommandInterceptor(int, char **) = 0; //Timer/Server Calls, users can never access these
	virtual bool userCommandInterceptor(int, char **, int) = 0; //User Calls

	//Any time an event happens, its passed here, see if you can do something with it
	virtual bool eventParser(IGameEvent *) = 0;

	//Stuff to track our plugin slot
	static int ModeNumber;
	static int getModeNumber(void);

protected:

	void modeStart(int **);
	bool modeEnd(int **, bool);

	void setModeLevel(int);

	void setModeName(char *);
	void setModeDesc(char *);
	
	bool isActive;
	int modeLevel;

	char * modeName;
	char * modeDesc;
	int modeNum;
};

#endif