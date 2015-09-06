#ifndef CMODEMAN_H
#define CMODEMAN_H

#include "CModeBase.h"
#include "eiface.h"
#include "edict.h"

class CModeMan
{
public:
	CModeMan();
	virtual ~CModeMan();

	bool userCommandExecute(char *, int, char **, int = -1); //Called though command man
	bool systemCommandExecute(char *, int, char **); //Called by timers and the system

	void eventManager(IGameEvent *);

	//Finder Functions
	int nameToSlot(char *);
	int getNumModes();

	//Function Array
	CModeBase ** ModeList; 

private:
	const int NumModes;
};

#endif