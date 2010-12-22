#include "Bdef.h"


void BI_Rsend(BLACSCONTEXT *ctxt, int dest, int msgid, BLACBUFF *bp)
{
   int info;

   BI_MPI_Rsend(bp->Buff, bp->N, bp->dtype, dest, msgid, ctxt->scp->comm, info);
}
