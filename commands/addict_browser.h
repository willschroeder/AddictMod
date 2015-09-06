#ifndef ADDICT_BROWSER_H
#define ADDICT_BROWSER_H

//Do Includes Here
#include "../CPlayer.h"
#include "../CServerMan.h"
#include "eiface.h"
#include "edict.h"
#include "../CCommandBase.h"
#include "addict_globals.h"


//#include "../addict/test/CDYSPlayer.h"

#define GAME_DLL 1 

//Include Externs Here
extern CServerMan * pServerMan;
extern IVEngineServer * engine;
extern userOptions * pCurrentSettings;

class addict_browser : public CCommandBase
{
public:
	addict_browser();
	virtual ~addict_browser();

	virtual bool onCommand(int, char **, int = -1); 

private:
	int * selectedPlayers;
	int selectedPlayersCount;
};

//Deals with the browser type
enum
{
	TYPE_TEXT = 0,  /* just display this plain text */
	TYPE_INDEX,     /* lookup text & title in stringtable */
	TYPE_URL,       /* show this URL */
	TYPE_FILE,      /* show this local file */
};

addict_browser::addict_browser() : CCommandBase()
{
	setCommandName("ad_browser");
	setCommandUsage("<access flag> <url> <visible (true/false)/#url to use musicURL> Opens the browser to the URL.");
	setCommandLevel(PLAYER_MANAGEMENT);
}

addict_browser::~addict_browser()
{
	//Deconstructor
}

bool addict_browser::onCommand(int numFlags, char ** flagList, int playerNum)
{

	//Check for CBase Block
	if(!pCurrentSettings->useCBaseFunctions || !pCurrentSettings->useCPlayerFunctions)
		return true;

	if(numFlags < 2)
	{
			if(playerNum != -1) engine->ClientPrintf(pServerMan->PlayerList[playerNum]->playerEdict,"Need more info to execute command.\n");
			return true;
	}

	commandStart(&selectedPlayers);

	if(Q_strcmp(flagList[0],"#self")==0)
	{
		selectedPlayers[0] = playerNum;
		selectedPlayersCount = 1;
	}
	else
		selectedPlayersCount = pServerMan->strToSlot(flagList[0],selectedPlayers); 

	//Return if None Found
	if(selectedPlayersCount == 0)
	{
		if(playerNum != -1) engine->ClientCommand(pServerMan->PlayerList[playerNum]->playerEdict,"No clients found matching that string!\n");
		return commandEnd(&selectedPlayers,true);
	}

	//Check for blank tag
	char * stringURL = NULL;
	bool isVisible = false;

	if(Q_strcmp(flagList[1],"#blank")==0)
	{
		stringURL = new char[20];
		Q_strcpy(stringURL,"about:blank"); //Will set the browser to a blank page
	} 
	else
	{
		if(numFlags > 2)
		{
			if(Q_strcmp(flagList[2],"true")==0)
				isVisible = true;
			else if(Q_strcmp(flagList[2],"#url")==0)
			{
				isVisible = false;
				int newLen = Q_strlen(flagList[1])+pCurrentSettings->musicUrlLen+1;
				stringURL = new char[newLen];
				Q_snprintf(stringURL,newLen,"%s%s",pCurrentSettings->musicUrl,flagList[1]);
				//Q_strcpy(stringURL,pCurrentSettings->musicUrl);
				//Q_strcat(stringURL,flagList[1],newLen);

				//Now announce the mp3 name
				if(Q_strcmp(flagList[0],"#all")==0)
				{
					dp_string temp(2);
					temp.addString("#all");
					temp.addString(flagList[1]);
					pCommandMan->commandExecute("ad_announce",temp.dpList,temp.getMaxItems());
				}
			}
		}

		if(stringURL == NULL)
		{
			stringURL = new char[Q_strlen(flagList[1])+1];
			Q_strcpy(stringURL,flagList[1]);
		}



		if(numFlags > 3)
			if(Q_strcmp(flagList[3],"true")==0)
				isVisible = true;

	}

	//Set up panel
	KeyValues * panel = new KeyValues("", "msg", stringURL, "title", "Addict Mod Browser");
	panel->SetInt("type", TYPE_URL);



	for(int i = 0; i < selectedPlayersCount; i++)
	{
		if(pServerMan->PlayerList[selectedPlayers[i]]->isABot)
			continue;

		CBasePlayer * player = dynamic_cast<CBasePlayer *>(pServerMan->PlayerList[selectedPlayers[i]]->playerEdict->GetUnknown());
		//CBasePlayer * player = (CBasePlayer *)pServerMan->PlayerList[selectedPlayers[i]]->playerEdict->GetUnknown();
		player->ShowViewPortPanel("info", isVisible, panel);
		
	}

	panel->deleteThis();
	delete stringURL;

	return commandEnd(&selectedPlayers,true);
}


#endif
