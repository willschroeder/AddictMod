#ifndef QUICK_COMMANDS_H
#define QUICK_COMMANDS_H

#include "eiface.h"
#include "edict.h"
#include "igameevents.h"

class quickCommands
{
public:
	
//	quickCommands();
//	~quickCommands();

	void commandInterceptor(IGameEvent *);
	static bool announceGlobal;

	
};

#endif