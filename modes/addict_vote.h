#ifndef ADDICT_VOTE_H
#define ADDICT_VOTE_H

#include "CModeBase.h"
#include "CPlayer.h"
#include "eiface.h"
#include "edict.h"
#include "addict_globals.h"
#include "CTimer.h"
#include "dp_string_tools.h"
#include "quick_commands.h"

extern CServerMan * pServerMan;
extern IVEngineServer	* engine;
extern userOptions * pCurrentSettings;
extern CTimerMan * pTimerMan;

enum PLAYER_VOTE_STATUS{CANT_VOTE,HASNT_VOTED,VOTED_NO,VOTED_YES};
enum PLAYER_VOTE{VOTE_NOTHING=0,VOTE_MAP,VOTE_SLAY,VOTE_KICK};

class addict_vote : public CModeBase
{

public:

	addict_vote();
	~addict_vote();

	virtual bool userCommandInterceptor(int, char **, int);	
	virtual bool systemCommandInterceptor(int, char **);
	virtual bool eventParser(IGameEvent *);

private:

	void prepArray(char *,int = -1);
	int setResetTimer(void);

	int * selectedPlayers;
	int selectedPlayersCount;
	int * voteList;

	int voteType;
	bool voteInProgress;
	bool votingEnabled;
	float lastVoteStarted;
	int voteTimers[2];
};

addict_vote::addict_vote()
{
	setModeName("ad_vote");
	setModeDesc("<map,slay,kick> <name> OR <stop> (Will kill current vote if admin)");
	setModeLevel(PLAYER_NORMAL);

	isActive = true;
	voteList = NULL;
	voteType = VOTE_NOTHING;
	voteInProgress = false;
	votingEnabled = false;
	lastVoteStarted = 0.0;
}

addict_vote::~addict_vote()
{
	delete voteList;
}

bool addict_vote::userCommandInterceptor(int numFlags, char ** flagList, int playerNum)
{
	if(numFlags < 1)
	{
		if(!quickCommands::announceGlobal) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		else
			engine->ServerCommand("say Need more info to execute command.\n");
		return true;
	}

	//Kill vote
	if(voteInProgress && Q_strcmp(flagList[0],"stop")==0 && pServerMan->PlayerList[playerNum]->adminLevel >= PLAYER_MANAGEMENT)
	{
		pTimerMan->destroyTimer(voteTimers[0]);
		pTimerMan->destroyTimer(voteTimers[1]);
		engine->ServerCommand("say Current vote canceled by admins.\n");

		//Now start timer to resume voting to stop votespam after cancel
		setResetTimer();
		
		voteInProgress = false; 
		votingEnabled = false;
		return true;
	}

	if(numFlags < 2)
	{
		if(!quickCommands::announceGlobal) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
		else
			engine->ServerCommand("say Need more info to execute command.\n");
		return true;
	}
	
	//Debug
	//if(voteInProgress)
	//	Warning("Vote is in progress\n");
	//else
	//	Warning("Vote not in progress\n");

	//if(votingEnabled)
	//	Warning("Vote is enabled\n");
	//else
	//	Warning("Vote is not enabled\n");
	
	modeStart(&selectedPlayers);

	char buffer[200];

		if(pServerMan->PlayerList[playerNum]->adminLevel >= pCurrentSettings->levelToVote)								
			if((!voteInProgress && votingEnabled) || (pServerMan->PlayerList[playerNum]->adminLevel >= PLAYER_MANAGEMENT && !voteInProgress))
			{

				//Different Options in here
					//Map
					if(Q_strcmp(flagList[0],"map")==0)
						if(engine->IsMapValid(flagList[1]))
						{
							voteType = VOTE_MAP;
							prepArray(flagList[1],playerNum);
							setResetTimer();
							Q_snprintf(buffer,200,"say A vote has started to change the level to %s, type yes or no to vote for this map. Needs %i percent to pass.\n",flagList[1],pCurrentSettings->votePassPercentage);
							engine->ServerCommand(buffer); // <-- Ghetto excuse not to write a command filter
							return modeEnd(&selectedPlayers,true);
						}
						else
						{
							if(!quickCommands::announceGlobal) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Map was not found, check the name.\n");
							else engine->ServerCommand("say Map was not found, check the name.\n");
							return modeEnd(&selectedPlayers,true);
						}

						//Below here is where you put functions dealing with people
						selectedPlayersCount = pServerMan->strToSlot(flagList[1],selectedPlayers); 
						if(selectedPlayersCount > 1)
						{
							if(!quickCommands::announceGlobal) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"More than one person matched the given string. Be more specific.\n");
							else engine->ServerCommand("say More than one person matched the given string. Be more specific.\n");
							return modeEnd(&selectedPlayers,true);
						}
						else if(selectedPlayersCount < 1)
						{
							if(!quickCommands::announceGlobal) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"No one was found matching the given string.\n");
							else engine->ServerCommand("say No one was found matching the given string.\n");
							return modeEnd(&selectedPlayers,true);
						}
						//Extra vars needed
						char nameSwitch[100];
						
						//Slay
						if(Q_strcmp(flagList[0],"slay")==0)
							if(pServerMan->PlayerList[selectedPlayers[0]]->adminLevel < PLAYER_MANAGEMENT) //<-- Maybe make this a changeable level in the cfg if needed
							{

								//Check for CBase Block
								if(!pCurrentSettings->useCBaseFunctions)
									return modeEnd(&selectedPlayers,true);

								voteType = VOTE_SLAY;
								prepArray(itoa(selectedPlayers[0],nameSwitch,10),playerNum);
								setResetTimer();
								Q_snprintf(buffer,200,"say A vote has started to slay %s, type yes or no to vote. Needs %i percent to pass.\n",pServerMan->PlayerList[selectedPlayers[0]]->playerName,pCurrentSettings->votePassPercentage);
								engine->ServerCommand(buffer); // <-- Ghetto excuse not to write a command filter
								return modeEnd(&selectedPlayers,true);
							}

						//Kick
						if(Q_strcmp(flagList[0],"kick")==0)
							if(pServerMan->PlayerList[selectedPlayers[0]]->adminLevel < PLAYER_MANAGEMENT) //<-- Maybe make this a changeable level in the cfg if needed
							{
								if(pServerMan->PlayerList[selectedPlayers[0]]->adminLevel >= PLAYER_PROTECTED && !pCurrentSettings->kickSameLevel)
									return modeEnd(&selectedPlayers,true);

								voteType = VOTE_KICK;
								prepArray(pServerMan->PlayerList[selectedPlayers[0]]->playerSteamID,playerNum);
								setResetTimer();
								Q_snprintf(buffer,200,"say A vote has started to kick %s, type yes or no to vote. Needs %i percent to pass.\n",pServerMan->PlayerList[selectedPlayers[0]]->playerName,pCurrentSettings->votePassPercentage);
								engine->ServerCommand(buffer); // <-- Ghetto excuse not to write a command filter
								return modeEnd(&selectedPlayers,true);
							}

				//End Different Options
			}
			else
			{
				if(!quickCommands::announceGlobal) 
					engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"It is too early to start a new vote.\n");
				else
					engine->ServerCommand("say It is too early to start a new vote.\n");

				return modeEnd(&selectedPlayers,true);
			}
		else
		{
			if(!quickCommands::announceGlobal) 
				engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"You are not high enough of a level to start votes.\n");

			return modeEnd(&selectedPlayers,true);
		}

	return modeEnd(&selectedPlayers,true);

}

bool addict_vote::systemCommandInterceptor(int numFlags, char ** flagList)
{

	if(Q_strcmp(flagList[0],"#mapstart")==0)
	{
		setResetTimer();
		voteInProgress = false; 
		votingEnabled = false;
		lastVoteStarted = 0.0;
		voteTimers[0] = -1;
		voteTimers[1] = -1;
	}


	if(Q_strcmp(flagList[0],"#voteEnable")==0)
	{
		votingEnabled = true;
		voteTimers[0] = -1;
		voteTimers[1] = -1;
	}

	if(Q_strcmp(flagList[0],"#voteEnd")==0)
	{

		//See if vote passed
		float votedYes = 0;
		float votedNo = 0;

		for(int i = 0; i < pServerMan->getMaxPlayers(); i++)
		{
			if(voteList[i] == VOTED_NO)
				votedNo++;

			else if(voteList[i] == VOTED_YES)
				votedYes++;

		}

		//Checks to make sure that there wont be division by 0 when tallying
		if((votedNo+votedYes) > 0)
		{
			//voted yes divided by total votes is greater than the vote % to pass, vote is passed
			float voteTally = ((votedYes/(votedNo+votedYes)) * 100);
			char buffer[200];
			if(voteTally >= pCurrentSettings->votePassPercentage)
			{
				lastVoteStarted = CTimerMan::getCurrentTime();

				if(voteType == VOTE_MAP)
				{
					lastVoteStarted = 0.0;
					quickCommands::announceGlobal = false;
					Q_snprintf(buffer,200,"say The vote passed to change the map to %s, changing map in 10 seconds.\n",flagList[1]);
					engine->ServerCommand(buffer);

					//10 second timer method
					int newTimer = pTimerMan->createTimer(10,"ad_map");
					pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(1);
					pTimerMan->timerList[newTimer]->cmdToExecute->addString(flagList[1]);
					pTimerMan->timerList[newTimer]->howToExec = COMMAND_STACK;

				}
				else if(voteType == VOTE_SLAY)
				{
					int playerID = Q_atoi(flagList[1]);
					if(pServerMan->PlayerList[playerID] != NULL)
					{
						if(pServerMan->PlayerList[playerID] != NULL)
						{
							dp_string temp(1);
							temp.addString(pServerMan->PlayerList[Q_atoi(flagList[1])]->playerName);
							pCommandMan->commandExecute("ad_slay",temp.dpList,temp.getMaxItems());

							Q_snprintf(buffer,200,"say Slaying %s because the vote passed.\n",pServerMan->PlayerList[Q_atoi(flagList[1])]->playerName,pCurrentSettings->votePassPercentage);
							engine->ServerCommand(buffer);
						}

					}
				}
				else if(voteType == VOTE_KICK)
				{
					int playerID = Q_atoi(flagList[1]);
					if(pServerMan->PlayerList[playerID] != NULL)
					{
						if(pServerMan->PlayerList[playerID] != NULL)
						{
							//Update to get steam id on vote, then ban it on sucess
						//	dp_string temp(2);
						//	temp.addString(pServerMan->PlayerList[playerID]->playerName);
						//	temp.addString("#voteban");
						//	pCommandMan->commandExecute("ad_ban",temp.dpList,temp.getMaxItems());

							Q_snprintf(buffer,200,"banid %i %s kick\n",pCurrentSettings->lengthOfVoteKickBan,flagList[1]);
							engine->ServerCommand(buffer);
							engine->ServerCommand("writeid\n");

							//Setting up 1 min delay timer
							int newTimer = pTimerMan->createTimer(60,"ad_kick");
							pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(1);
							pTimerMan->timerList[newTimer]->cmdToExecute->addString(flagList[1]);
							pTimerMan->timerList[newTimer]->howToExec = COMMAND_STACK;

							Q_snprintf(buffer,200,"say Kicking & banning steamID %s for %i minutes.\n",flagList[1],pCurrentSettings->lengthOfVoteKickBan);
							engine->ServerCommand(buffer);
						}
					}
				}
				else 
					engine->ServerCommand("say I have made a terrible mistake, and forgot what we were voting on.\n");

				voteInProgress = false;
				return true;
			}

			//Should only reach here if the vote fails?
			Q_snprintf(buffer,100,"say The proposed vote did not pass, needed %i percent but only got %i percent.\n",pCurrentSettings->votePassPercentage,int(voteTally));
			engine->ServerCommand(buffer);
			return true;
		}
	}
	return false;
}

bool addict_vote::eventParser(IGameEvent * anEvent)
{
	if(Q_strcmp("player_say",anEvent->GetName())==0 && voteInProgress)
	{
		const char * saidText = anEvent->GetString("text");
		int playerNum = pServerMan->indexToSlot(anEvent->GetInt("userid"));


			if(Q_strcmp(saidText,"yes")==0)
				if(voteList[playerNum] >= HASNT_VOTED)
				{
					voteList[playerNum] = VOTED_YES;
					return true;
				}

			if(Q_strcmp(saidText,"no")==0)
				if(voteList[playerNum] >= HASNT_VOTED)
				{
					voteList[playerNum] = VOTED_NO;
					return true;
				}
	}

	return true;
}

void addict_vote::prepArray(char * voteObject,int voteStarter)
{

	if(voteList == NULL)
	{
		voteList = new int[pServerMan->getMaxPlayers()];

		for(int i = 0; i < pServerMan->getMaxPlayers(); i++)
			voteList[i] = 0;
	}

	//Reset Voting Lists and timers, help fight mystery 2 votes at a time
	pTimerMan->destroyTimer(voteTimers[0]);
	pTimerMan->destroyTimer(voteTimers[1]);

	//Set Up Vote
	for(int i = 0; i < pServerMan->getMaxPlayers(); i++)
		if(pServerMan->PlayerList[i] != NULL)
		{
			if(pServerMan->PlayerList[i]->adminLevel >= pCurrentSettings->levelToVote)
				voteList[i] = HASNT_VOTED;
			else
				voteList[i] = CANT_VOTE;
		}
		else
			voteList[i] = CANT_VOTE;

	//Set the voter to vote yes for his own vote
	if(voteStarter != -1)
		voteList[voteStarter] = VOTED_YES;

	voteInProgress = true;

	//Set End Timer
	int newTimer = pTimerMan->createTimer(pCurrentSettings->voteTimeLength,"ad_vote");
	pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(2);
	pTimerMan->timerList[newTimer]->cmdToExecute->addString("#voteEnd");
	pTimerMan->timerList[newTimer]->cmdToExecute->addString(voteObject);
	pTimerMan->timerList[newTimer]->howToExec = MODE_STACK;
	voteTimers[0] = newTimer;

	//Set up 15 second warning timer
	char buffer[200];
	switch(voteType)
	{
	case VOTE_MAP:
		Q_snprintf(buffer,200,"The current vote to change the map to %s ends in 15 seconds. Say yes or no to vote.",voteObject);
		break;
	case VOTE_SLAY:
		Q_snprintf(buffer,200,"The current vote to slay %s ends in 15 seconds. Say yes or no to vote.",pServerMan->PlayerList[Q_atoi(voteObject)]->playerName);
		break;
	case VOTE_KICK:
		Q_snprintf(buffer,200,"The current kick %s ends in 15 seconds. Say yes or no to vote.",voteObject);
		break;
	default:
	   return; //Should never reach here
	}
	newTimer = pTimerMan->createTimer(pCurrentSettings->voteTimeLength-15,"ad_say");
	pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(1);
	pTimerMan->timerList[newTimer]->cmdToExecute->addString(buffer);
	pTimerMan->timerList[newTimer]->howToExec = COMMAND_STACK;
	voteTimers[1] = newTimer;

	//Set up announce about a new vote
	dp_string temp(2);
	temp.addString("#all");
	temp.addString("A vote has started!");
	pCommandMan->commandExecute("ad_announce",temp.dpList,temp.getMaxItems());

}

int addict_vote::setResetTimer(void)
{
	lastVoteStarted = CTimerMan::getCurrentTime();

	//Set reset timer
	int newTimer = pTimerMan->createTimer(pCurrentSettings->timeBetweenVotes,"ad_vote");
	pTimerMan->timerList[newTimer]->cmdToExecute = new dp_string(1);
	pTimerMan->timerList[newTimer]->cmdToExecute->addString("#voteEnable");
	pTimerMan->timerList[newTimer]->howToExec = MODE_STACK;
	return newTimer;
}

#endif