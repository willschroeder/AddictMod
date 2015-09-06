#ifndef CCOMMANDMAN_H
#define CCOMMANDMAN_H


#include "CCommandBase.h"
#include "eiface.h"
#include "edict.h"

class CCommandMan
{

public:

	CCommandMan();
	virtual ~CCommandMan();

	PLUGIN_RESULT clientCommand(edict_t *);
	bool commandExecute(char *, char **, int, int = -1);

	//Finder Functions
	int nameToSlot(char *);
	int getNumCommands();

	//Function Array
	CCommandBase ** CommandList; 

private:
	const int NumCommands;

};

#endif