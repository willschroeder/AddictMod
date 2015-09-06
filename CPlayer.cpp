#include "CPlayer.h"
#include "edict.h"
#include "eiface.h"
#include "addict_globals.h"
#include "CCommandMan.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern IVEngineServer	* engine;
extern CCommandMan * pCommandMan;

int CPlayer::CurrentNumPlayers = 0;

//Our magical Constructor!
CPlayer::CPlayer(edict_t *pEntity, const char *pszName, const char *pszAddress, bool isBot)
{
	//Set Entity Pointer
	playerEdict = pEntity;

	//Set Address
	setPlayerIpAddress(pszAddress);

	//Not admin until is
	adminLevel = PLAYER_NORMAL;

	//Get Player Edict
	playerIndex = engine->GetPlayerUserId(pEntity);

	//Set Player State
	playerState = PLAYER_LIMBO; 

	//Set Player Team, 0 for connecting
	playerTeam = TEAM_NONE;

	//Set Player Name
	playerAdminName = NULL;
	playerName = NULL;
	setPlayerName(pszName);

	//Set Steam ID, or deal with bot
	if (isBot)
		setPlayerBot(true);
	else
	{
		setPlayerSteamID(engine->GetPlayerNetworkIDString(pEntity));
		setPlayerBot(false);
	}

	//User tracking
	blockConsoleCommands = true;

	//Update Static
	CurrentNumPlayers++;

}

CPlayer::~CPlayer(void)
{
	delete playerName;
	delete playerSteamID;
	delete playerIpAddress;
	
	if(playerAdminName != NULL)
		delete playerAdminName;

	CurrentNumPlayers--;
}

void CPlayer::setPlayerName(const char * name)
{
	if (playerName != NULL)
		delete playerName;
	
	playerName = new char[Q_strlen(name)+1]; 
	Q_strcpy(playerName,name);
}

void CPlayer::setPlayerSteamID(const char * steamId)
{
	playerSteamID = new char[Q_strlen(steamId)+1];
	Q_strcpy(playerSteamID,steamId);
}

void CPlayer::setPlayerState(int n)
{
	playerState = n;
}

void CPlayer::setPlayerIpAddress(const char * s)
{	
	playerIpAddress = new char[Q_strlen(s)+1];
	Q_strcpy(playerIpAddress,s);
}

void CPlayer::setPlayerBot(bool isBot)
{
	if (isBot)
	{
		isABot = true;
		setPlayerSteamID("BOT");
	} 
	else
		isABot = false;

}

void CPlayer::setPlayerAdminName(const char * s)
{
	if(playerAdminName != NULL)
		delete playerAdminName;

	playerAdminName = new char[Q_strlen(s)+1];
	Q_strcpy(playerAdminName,s);
}