#include "Bdef.h"
void BI_zMPI_amx2(void *in, void *inout, int *N, BI_MPI_Datatype *dtype)
{
   void BI_zvvamx2(int, char *, char *);
   BI_zvvamx2(*N, inout, in);
}
