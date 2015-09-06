#ifndef CSERVERMAN_H
#define CSERVERMAN_H

#include "CPlayer.h"
#include "eiface.h"
#include "edict.h"
#include "igameevents.h"

class CServerMan
{

public:

	//Constructors
	CServerMan(void);
	~CServerMan(void);

	//Add & Remove Player
	int playerConnect(edict_t *pEntity, const char *pszName, const char *pszAddress,bool isBot);
	bool playerDisconnect(edict_t *pEntity);
	void networkIdVerified(const char *pszUserName, const char *pszNetworkID, bool serverRefresh = false);

	//Resolver Functions
	int entityToSlot(edict_t * pEntity);
	int indexToSlot(int);
	int strToSlot(const char *, int *);

	//Server Info

	char * currentMapName;
    
	//The CPlayer Array 
	CPlayer ** PlayerList;

	//My Tools
	void isAdmin(int);
	int getMaxPlayers(void);

	//Server Functions
	void levelInit(const char *);
	void serverActivate(int);
	void serverGameEvent( IGameEvent * );

private:
	int MaxPlayers;

};



#endif