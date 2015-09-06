#ifndef ADDICT_WORD_FILTER_H
#define ADDICT_WORD_FILTER_H

#include "CModeBase.h"
#include "CPlayer.h"
#include "eiface.h"
#include "edict.h"
#include "addict_globals.h"

extern CServerMan * pServerMan;
extern CCommandMan * pCommandMan;
extern IVEngineServer	* engine;

class addict_wordfilter : public CModeBase
{

public:

	addict_wordfilter();
	~addict_wordfilter();

	virtual bool userCommandInterceptor(int, char **, int);	
	virtual bool systemCommandInterceptor(int, char **);
	virtual bool eventParser(IGameEvent *);

private:
	int * selectedPlayers;
	int selectedPlayersCount;

	char * kickWord;

};

addict_wordfilter::addict_wordfilter()
{
	setModeName("ad_wordfilter");
	setModeDesc("<activated true/false> <word> Kicks a person if they say a certain word.");
	setModeLevel(PLAYER_MANAGEMENT);

	isActive = false;
	kickWord = NULL;
}

addict_wordfilter::~addict_wordfilter()
{
	if(kickWord != NULL)
		delete kickWord;
}

bool addict_wordfilter::userCommandInterceptor(int numFlags, char ** flagList, int playerNum)
{

	if(numFlags < 1)
	{	
		if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		return true;
	}

	if(Q_strcmp(flagList[0],"true")==0)
	{
		if(numFlags < 2)
		{	
			if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need a word to filter.\n");
			return true;
		}
		kickWord = new char[Q_strlen(flagList[1])+1];
		Q_strcpy(kickWord,flagList[1]);

		char buffer[100];
		Q_snprintf(buffer,100,"say If you say the word %s, you are getting kicked.\n",kickWord);
		engine->ServerCommand(buffer);
		isActive = true;
		return true;
	}
	else if(Q_strcmp(flagList[0],"false")==0)
	{
		isActive = false;

		if(kickWord != NULL)
			delete kickWord;

		return true;
	}

	return true;
}

bool addict_wordfilter::systemCommandInterceptor(int numFlags, char ** flagList)
{
	//Blank works on map start
	if(Q_strcmp("#mapstart",flagList[0])==0)
	{
		isActive = false;

		if(kickWord != NULL)
			delete kickWord;
	}

	return true;
}

bool addict_wordfilter::eventParser(IGameEvent * anEvent)
{
	if(Q_strcmp("player_say",anEvent->GetName())==0)
	{
		const char * rawString = anEvent->GetString("text");
		if(Q_strcmp(rawString,kickWord)==0)
		{
			dp_string temp(1);
			temp.addString(pServerMan->PlayerList[pServerMan->indexToSlot(anEvent->GetInt("userid"))]->playerSteamID);
			pCommandMan->commandExecute("ad_kick",temp.dpList,temp.getMaxItems());

			char buffer[100];
			Q_snprintf(buffer,100,"say If you say the word %s, you are getting kicked.\n",kickWord);
			engine->ServerCommand(buffer);
			return true;
		}
	}
	return true;
}

#endif