#ifndef CDYSPLAYER_H
#define CDYSPLAYER_H

#include "basecombatcharacter.h"
#include "usercmd.h"
#include "playerlocaldata.h"
#include "PlayerState.h"
#include "dlls/iplayerinfo.h"

#include "recipientfilter.h"
#include "MRecipentFilter.h"
#include "../addict/CServerMan.h"
extern CServerMan * pServerMan;

class CDYSPlayer
{
public:
	void ShowViewPortPanel( const char * name, bool bShow, KeyValues *data)
	{
		MRecipientFilter filter;
		filter.AddAllPlayers(pServerMan->getMaxPlayers());
	//	filter.MakeReliable();

		int count = 0;
		KeyValues *subkey = NULL;

		if ( data )
		{
			subkey = data->GetFirstSubKey();
			while ( subkey )
			{
				count++; subkey = subkey->GetNextKey();
			}

			subkey = data->GetFirstSubKey(); // reset 
		}

		UserMessageBegin( filter, "VGUIMenu" );
		WRITE_STRING( name ); // menu name
		WRITE_BYTE( bShow?1:0 );
		WRITE_BYTE( count );

		// write additional data (be carefull not more than 192 bytes!)
		while ( subkey )
		{
			WRITE_STRING( subkey->GetName() );
			WRITE_STRING( subkey->GetString() );
			subkey = subkey->GetNextKey();
		}
		MessageEnd();
	}
};

#endif