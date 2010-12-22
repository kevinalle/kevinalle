#include "Bdef.h"

void BI_BlacsAbort(int ErrNo)
{
   int ierr;
   fflush(stderr);
   fflush(stdout);
   BI_MPI_Abort(BI_MPI_COMM_WORLD, ErrNo, ierr);
}
