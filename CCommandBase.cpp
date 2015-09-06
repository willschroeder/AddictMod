#include "CPlayer.h"
#include "CServerMan.h"
#include "CCommandBase.h"

#include "eiface.h"
#include "edict.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern CServerMan * pServerMan;

int CCommandBase::CommandNumber = 0;

CCommandBase::CCommandBase()
{
	commandName = commandUsage = NULL;
	commandNum = getCommandNumber();
}

CCommandBase::~CCommandBase()
{
	delete commandName;
	delete commandUsage;
}

//The Gets
char * CCommandBase::getCommandName(void)
{
	return commandName;
}

char * CCommandBase::getCommandUsage(void)
{
	return commandUsage;
}

int CCommandBase::getCommandLevel(void)
{
	return commandLevel;
}

//The Sets
void CCommandBase::setCommandName(char * s)
{
	if(commandName != NULL)
		delete commandName;

	commandName = new char[Q_strlen(s)+1];
	Q_strcpy(commandName,s);
}

void CCommandBase::setCommandUsage(char * s)
{
	if(commandUsage != NULL)
		delete commandUsage;

	commandUsage = new char[Q_strlen(s)+1];
	Q_strcpy(commandUsage,s);
}

void CCommandBase::setCommandLevel(int b)
{
	commandLevel = b;
}

//Command Start/End
void CCommandBase::commandStart(int ** selectedPlayers)
{
	*selectedPlayers = new int[CPlayer::CurrentNumPlayers+1];
}

bool CCommandBase::commandEnd(int ** selectedPlayers, bool result)
{
	delete *selectedPlayers;
	return result;
}

//Static Functions
int CCommandBase::getCommandNumber(void)
{
	return CCommandBase::CommandNumber++; 
}

