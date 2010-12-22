#include "Bdef.h"
BI_MPI_Datatype BI_GetMpiGeType(BLACSCONTEXT *ctxt, int m, int n, int lda,
                                BI_MPI_Datatype Dtype, int *N)
{
   int info;
   BI_MPI_Datatype GeType;

/*
 * Some versions of mpich and its derivitives cannot handle 0 byte typedefs,
 * so we set type MPI_BYTE as a flag for a 0 byte message
 */
#ifdef ZeroByteTypeBug
   if ( (m < 1) || (n < 1) )
   {
      *N = 0;
      return (BI_MPI_BYTE);
   }
#endif
   *N = 1;
   BI_MPI_Type_vector(n, m, lda, Dtype, &GeType, info);
   BI_MPI_Type_commit(&GeType, info);

   return(GeType);
}
