#ifndef CPLAYER_H
#define CPLAYER_H

#include "CPlayer.h"
#include "edict.h"

//Player States
enum PLAYER_TEAM {TEAM_NONE = 0,TEAM_SPEC,TEAM_T,TEAM_CT};

class CPlayer
{

public:

	edict_t * playerEdict; //Pointer to Edict
	int playerIndex;
	int playerState; 
	int playerTeam;
	int adminLevel;
	bool isABot;
	char * playerIpAddress; 
	char * playerName; 
	char * playerSteamID;
	char * playerAdminName;

	//Stats to keep on players
	bool blockConsoleCommands;

	CPlayer(edict_t *pEntity, const char *pszName, const char *pszAddress, bool isBot); 
	~CPlayer(void);

	//TODO: Return alias to self for chaining CPlayer &
	void setPlayerName(const char *);
	void setPlayerSteamID(const char *);
	void setPlayerState(int); //PLAYER_STATE = PLAYER_LIMBO,PLAYER_SPECTATING,PLAYER_DEAD,PLAYER_ALIVE
	void setPlayerIpAddress(const char *);
	void setPlayerBot(bool);
	void setPlayerAdminName(const char *);


	//Static Num for Counting Num Players
	static int CurrentNumPlayers;

private:

};

#endif