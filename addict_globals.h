#ifndef ADDICT_GLOBALS_H
#define ADDICT_GLOBALS_H

//My steamID
#define ADDICT_STEAM_ID "STEAM_0:0:4489138"

//As much as this pains me, we need the STL here
#include <string>
#include <fstream>

using namespace std;

#include "CCommandMan.h"
#include "CServerMan.h"
#include "CPlayer.h"
#include "eiface.h"
#include "edict.h"
#include "dp_string_tools.h"

extern CServerMan * pServerMan;
extern IVEngineServer * engine;
extern CCommandMan * pCommandMan;

//player access levels
enum PLAYER_ACCESS{PLAYER_NORMAL=0,PLAYER_PROTECTED,PLAYER_MANAGEMENT,PLAYER_TOTAL,DEV_ONLY};
enum PLAYER_STATE {PLAYER_LIMBO = 0,PLAYER_SPECTATING,PLAYER_DEAD,PLAYER_ALIVE};

//Options struct
struct userOptions
{
	userOptions()
	{
		levelToVote = 1;
		lengthOfVoteKickBan = 5;
		timeBetweenVotes = 5;
		votePassPercentage = 50;
		voteTimeLength = 20;
		useReservedSlots = false;
		useCharFilter = false;
		musicUrl = NULL;
		musicUrlLen = 0;
		numQuickUrl = 0;
		quickUrl = NULL;
		quickUrlName = NULL;
		useMapRandomizer = true;
		kickSameLevel = true;
		useCBaseFunctions = true;
		useCPlayerFunctions = true;
		tkPunish = 0;
		userFile = NULL;
	}

	~userOptions()
	{
		if(musicUrl != NULL)
			delete musicUrl;

		if(quickUrl != NULL)
			delete quickUrl;

		if(quickUrl != NULL)
			delete quickUrlName;

	}

	int levelToVote;
	int lengthOfVoteKickBan;
	float timeBetweenVotes;
	int votePassPercentage;
	float voteTimeLength;
	bool useReservedSlots;
	bool useCharFilter;
	bool useMapRandomizer;
	char * musicUrl;
	char * userFile;
	int musicUrlLen;
	bool kickSameLevel;
	bool useCBaseFunctions;
	bool useCPlayerFunctions;
	int tkPunish;

	int numQuickUrl;
	dp_string * quickUrl;
	dp_string * quickUrlName;
};

//my utils, never initialized/static functions only 
class adUtils
{
public:

	static bool charCheck(const char * pszName)
	{
		for(int i = 0; pszName[i] != '\0';i++)
			if(pszName[i] < 33 || pszName[i] > 126)
				if(pszName[i] != ' ')
					return true;

		return false;
	}

	static void populateSettings(userOptions * currentSettings)
	{
		//do cleanup for reload
		if(currentSettings->quickUrl != NULL)
			delete currentSettings->quickUrl;

		if(currentSettings->quickUrlName != NULL)
			delete currentSettings->quickUrlName;

		if(currentSettings->musicUrl != NULL)
			delete currentSettings->musicUrl;

		if(currentSettings->userFile != NULL)
			delete currentSettings->userFile;

		string curLine;

		char buffer[100];
		engine->GetGameDir(buffer,100);
		curLine.append(buffer);
		curLine.append("\\addons\\addict_mod.ini");
		fstream adminFile(curLine.c_str());

		if(adminFile.is_open())
		{
			while(!adminFile.eof())
			{
				getline(adminFile,curLine);

				if (curLine.empty())
					continue;

				const char * charLine = curLine.c_str();

				//Break if commented line
				if(charLine[0]=='/' && charLine[1]=='/')
				continue;

				//Remember, when comparing do the num one less than the total of chars

				if(Q_strncmp(charLine,"levelToVote=",11)==0)
					currentSettings->levelToVote = Q_atoi(charLine+12);

				if(Q_strncmp(charLine,"lengthOfVoteKickBans=",20)==0)
					currentSettings->timeBetweenVotes = Q_atoi(charLine+21);

				if(Q_strncmp(charLine,"timeBetweenVotes=",16)==0)
					currentSettings->timeBetweenVotes = Q_atof(charLine+17);

				if(Q_strncmp(charLine,"voteTimeLength=",14)==0)
					currentSettings->voteTimeLength = Q_atof(charLine+15);

				if(Q_strncmp(charLine,"tkPunish=",8)==0)
					currentSettings->tkPunish = Q_atoi(charLine+9);

				if(Q_strncmp(charLine,"votePassPercentage=",18)==0)
					currentSettings->votePassPercentage = Q_atoi(charLine+19);

				if(Q_strncmp(charLine,"useReservedSlots=",16)==0)
					if(Q_atoi(charLine+17) == 0)
						currentSettings->useReservedSlots = false;
					else
						currentSettings->useReservedSlots = true;

				if(Q_strncmp(charLine,"useCharFilter=",13)==0)
					if(Q_atoi(charLine+14) == 0)
						currentSettings->useCharFilter = false;
					else
						currentSettings->useCharFilter = true;

				if(Q_strncmp(charLine,"useMapRandomizer=",16)==0)
					if(Q_atoi(charLine+17) == 0)
						currentSettings->useMapRandomizer = false;
					else
						currentSettings->useMapRandomizer = true;

				if(Q_strncmp(charLine,"kickSameLevel=",13)==0)
					if(Q_atoi(charLine+14) == 0)
						currentSettings->kickSameLevel = false;
					else
						currentSettings->kickSameLevel = true;


				if(Q_strncmp(charLine,"useCBaseFunctions=",17)==0)
					if(Q_atoi(charLine+18) == 0)
						currentSettings->useCBaseFunctions = false;
					else
						currentSettings->useCBaseFunctions = true;

				if(Q_strncmp(charLine,"useCPlayerFunctions=",19)==0)
					if(Q_atoi(charLine+20) == 0)
						currentSettings->useCPlayerFunctions = false;
					else
						currentSettings->useCPlayerFunctions = true;

				if(Q_strncmp(charLine,"musicUrl=",8)==0)
				{
					currentSettings->musicUrl = new char[Q_strlen(charLine)-8];
					Q_strcpy(currentSettings->musicUrl,charLine+9);
					currentSettings->musicUrlLen = Q_strlen(currentSettings->musicUrl);
				}

				if(Q_strncmp(charLine,"userFile=",8)==0)
				{
					currentSettings->userFile = new char[Q_strlen(charLine)];
					Q_strcpy(currentSettings->userFile,charLine+9);
				}

				//Quick Command URL's
				if(Q_strncmp(charLine,"numQuickUrl=",11)==0)
				{
					currentSettings->numQuickUrl = Q_atoi(charLine+12);
					currentSettings->quickUrl = new dp_string(currentSettings->numQuickUrl);
					currentSettings->quickUrlName = new dp_string(currentSettings->numQuickUrl);
				}

				if(Q_strncmp(charLine,"nameQuickUrl=",11)==0)
				{
					if(currentSettings->numQuickUrl != 0)
						currentSettings->quickUrlName->addString(charLine+13);
				}

				if(Q_strncmp(charLine,"quickUrl=",8)==0)
				{
					if(currentSettings->numQuickUrl != 0)
						currentSettings->quickUrl->addString(charLine+9);
				}

			}

		}
		else
			Msg("Unable to open addict_mod.ini\n");

	}

	static int getPlayerAccessLevel(char * steamID,int playerSlot,userOptions * currentSettings)
	{

		string curLine;
		fstream adminFile;
		if(currentSettings->userFile == NULL)
		{
			char buffer[100];
			engine->GetGameDir(buffer,100);
			curLine.append(buffer);
			curLine.append("\\addons\\addict_users.ini");
			adminFile.open(curLine.c_str());
		}
		else
			adminFile.open(currentSettings->userFile);

		int i;
		bool wasMismatch;

		if(adminFile.is_open())
		{
			while(!adminFile.eof())
			{
				getline(adminFile,curLine);

				//Continue if blank line
				if(curLine.empty())
					continue;

				const char * charLine = curLine.c_str();
				wasMismatch = false;

				//Continue if commented line
				if(charLine[0]=='/' && charLine[1]=='/')
					continue;

				//Check for Match
				for(i = 5; charLine[i] != '|';i++) //Start at 5 to skip the STEAM
					if(charLine[i] != steamID[i])
					{
						wasMismatch = true;
						break;
					}

				//If mismatch, next name
				if(wasMismatch)
					continue;

				//Match Found, we can slow down now
				int playerLevel = Q_atoi(charLine+(Q_strlen(charLine)-1));
				
				if(playerLevel < 0 || playerLevel > 3)
					playerLevel = 0;

				//Set admin level
				pServerMan->PlayerList[playerSlot]->adminLevel = playerLevel;

				//Set admin name
				char temp[100];
				int j;
				i++;
				int counter= 0;
				for(j = i; charLine[j] != '|';j++)
					counter++;

				Q_strncpy(temp,charLine+i,++counter);

				pServerMan->PlayerList[playerSlot]->setPlayerAdminName(temp);
				adminFile.close();
				return playerLevel;
				
			}
		}
		adminFile.close();
		return 0; //User not found, so they are a pubbie
	}

	static void serverExec(char * s)
	{
		//ad_exec #server s
		dp_string temp(2);
		temp.addString("#server");
		temp.addString(s);
		pCommandMan->commandExecute("ad_exec",temp.dpList,temp.getMaxItems());
	}

};

#endif

