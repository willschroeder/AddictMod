#include "CPlayer.h"
#include "CServerMan.h"
#include "CModeBase.h"

#include "eiface.h"
#include "edict.h"
#include "igameevents.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern CServerMan * pServerMan;

int CModeBase::ModeNumber = 0;

CModeBase::CModeBase()
{
	modeName = modeDesc = NULL;
	modeNum = getModeNumber();
	isActive = false;
}

CModeBase::~CModeBase()
{
	//Professor Hax 
}

char * CModeBase::getModeName()
{
	return modeName;
}
char * CModeBase::getModeDesc()
{
	return modeDesc;
}

bool CModeBase::getModeActive()
{
	return isActive;
}

//The Sets
void CModeBase::setModeName(char * s)
{
	if(modeName != NULL)
		delete modeName;

	modeName = new char[Q_strlen(s)+1];
	Q_strcpy(modeName,s);
}

void CModeBase::setModeDesc(char * s)
{
	if(modeDesc != NULL)
		delete modeDesc;

	modeDesc = new char[Q_strlen(s)+1];
	Q_strcpy(modeDesc,s);
}

int CModeBase::getModeNumber(void)
{
	return CModeBase::ModeNumber++;
}

//Command Start/End
void CModeBase::modeStart(int ** selectedPlayers)
{
	*selectedPlayers = new int[CPlayer::CurrentNumPlayers+1];
}

bool CModeBase::modeEnd(int ** selectedPlayers, bool result)
{
	delete *selectedPlayers;
	return result;
}

//Mode Levels
void CModeBase::setModeLevel(int n)
{
	modeLevel = n;
}

int CModeBase::getModeLevel()
{
	return modeLevel;
}