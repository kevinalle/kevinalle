#include "Bdef.h"
void BI_cMPI_sum(void *in, void *inout, int *N, BI_MPI_Datatype *dtype)
{
   void BI_cvvsum(int, char *, char *);
   BI_cvvsum(*N, inout, in);
}
