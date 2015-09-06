#ifndef COMMANDBASE_H
#define COMMANDBASE_H

#include "eiface.h"
#include "edict.h"

//This class is the base for all one time commands. Kick, Slay, Damage. 
//They can be used in conjunction with the timer class, but still code them like a one time thing

class CCommandBase
{

public:
	CCommandBase();
	CCommandBase(char * name, char * usage, bool isAdmin);
	virtual ~CCommandBase();

	char * getCommandName(void);
	char * getCommandUsage(void);
	int getCommandLevel(void);

	virtual bool onCommand(int, char **, int = -1) = 0; //if player num is -1, the command was executed by the server

	//Stuff to track our plugin slot
	static int CommandNumber;
	static int getCommandNumber(void);
	

protected:

	//Command Start/End
	void commandStart(int **);
	bool commandEnd(int **, bool);

	//Functions
	void setCommandName(char *);
	void setCommandUsage(char *);
	void setCommandLevel(int);

	//Base Vars
	char * commandName;
	char * commandUsage;
	int commandLevel;
	int commandNum;
};

#endif