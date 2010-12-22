#include "Bdef.h"

void BI_Srecv(BLACSCONTEXT *ctxt, int src, int msgid, BLACBUFF *bp)
{
   int i, info;
   extern BI_MPI_Status *BI_Stats;

   BI_MPI_Recv(bp->Buff, bp->N, bp->dtype, src, msgid, ctxt->scp->comm,
               BI_Stats, info);
/*
 * If we are doing our own buffering, need to determine the true length of
 * the message just received
 */
#ifndef MpiBuffGood
   if (bp->dtype == BI_MPI_PACKED)
   {
      BI_MPI_Get_count(BI_Stats, BI_MPI_PACKED, &i, info);
      if (i != BI_MPI_UNDEFINED) bp->N = i;
      else BI_BlacsWarn(BI_ContxtNum(ctxt), __LINE__, __FILE__,
                        "MPI_Get_count returned MPI_UNDEFINED.\n");
   }
#endif
}
