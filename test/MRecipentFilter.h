#ifndef _MRECIIENT_FILTER_H
#define _MRECIPIENT_FILTER_H
#include "irecipientfilter.h"
#include "bitvec.h"
#include "tier1/utlvector.h"

//http://www.hl2coding.com/forums/viewtopic.php?t=31

class MRecipientFilter :
   public IRecipientFilter
{
public:
   MRecipientFilter(void);
   ~MRecipientFilter(void);

   virtual bool IsReliable( void ) const;
   virtual bool IsInitMessage( void ) const;

   virtual int GetRecipientCount( void ) const;
   virtual int GetRecipientIndex( int slot ) const;
   void AddAllPlayers( int maxClients );


private:
   bool m_bReliable;
   bool m_bInitMessage;
   CUtlVector< int > m_Recipients;
};

#endif 

