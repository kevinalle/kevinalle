#include "Bdef.h"

void BI_Unpack(BLACSCONTEXT *ctxt, BVOID *A, BLACBUFF *bp, BI_MPI_Datatype Dtype)
{
   int i=0, info, one=1;

/*
 * Some versions of mpich and its derivitives cannot handle 0 byte typedefs,
 * so we have set MPI_BYTE as a flag for a 0 byte message
 */
#ifdef ZeroByteTypeBug
   if (Dtype == BI_MPI_BYTE) return;
#endif
   BI_MPI_Unpack(bp->Buff, bp->Len, &i, A, one, Dtype, ctxt->scp->comm, info);
   BI_MPI_Type_free(&Dtype, info);
}
