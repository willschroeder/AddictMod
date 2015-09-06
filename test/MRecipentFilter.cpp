#include "MRecipientFilter.h"
#include "interface.h"
#include "filesystem.h"
#include "engine/iserverplugin.h"
#include "dlls/iplayerinfo.h"
#include "eiface.h"
#include "igameevents.h"
#include "convar.h"
#include "Color.h"

#include "shake.h"
#include "IEffects.h"
#include "engine/IEngineSound.h"

extern IVEngineServer   *engine;
extern IPlayerInfoManager *playerinfomanager;
extern IServerPluginHelpers *helpers;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

MRecipientFilter::MRecipientFilter(void)
{
}

MRecipientFilter::~MRecipientFilter(void)
{
}

int MRecipientFilter::GetRecipientCount() const
{
   return m_Recipients.Size();
}

int MRecipientFilter::GetRecipientIndex(int slot) const
{
   if ( slot < 0 || slot >= GetRecipientCount() )
      return -1;

   return m_Recipients[ slot ];
}

bool MRecipientFilter::IsInitMessage() const
{
   return false;
}

bool MRecipientFilter::IsReliable() const
{
   return false;
}

void MRecipientFilter::AddAllPlayers(int maxClients)
{
   m_Recipients.RemoveAll();
   int i;
   for ( i = 1; i <= maxClients; i++ )
   {
      edict_t *pPlayer = engine->PEntityOfEntIndex(i);
      if ( !pPlayer || pPlayer->IsFree()) {
         continue;
      }
      //AddRecipient( pPlayer );
      m_Recipients.AddToTail(i);
   }
} 

