#include "Bdef.h"

#if (INTFACE == C_CALL)
void Cblacs_pinfo(int *mypnum, int *nprocs)
#else
F_VOID_FUNC blacs_pinfo_(int *mypnum, int *nprocs)
#endif
{
   int ierr;
   extern int BI_Iam, BI_Np;

/*
 * If this is our first call, will need to set up some stuff
 */
   if (BI_F77_MPI_COMM_WORLD == NULL)
   {
/*
 *    The BLACS always call f77's mpi_init.  If the user is using C, he should
 *    explicitly call MPI_Init . . .
 */
      MPI_Initialized(nprocs);
#ifdef MainInF77
      if (!(*nprocs)) bi_f77_init_();
#else
      if (!(*nprocs))
         BI_BlacsErr(-1, -1, __FILE__,
            "Users with C main programs must explicitly call MPI_Init");
#endif
      BI_F77_MPI_COMM_WORLD = (int *) malloc(sizeof(int));
#ifdef UseF77Mpi
      BI_F77_MPI_CONSTANTS = (int *) malloc(23*sizeof(int));
      ierr = 1;
      bi_f77_get_constants_(BI_F77_MPI_COMM_WORLD, &ierr, BI_F77_MPI_CONSTANTS);
#else
      ierr = 0;
      bi_f77_get_constants_(BI_F77_MPI_COMM_WORLD, &ierr, nprocs);
#endif
      BI_MPI_Comm_size(BI_MPI_COMM_WORLD, &BI_Np, ierr);
      BI_MPI_Comm_rank(BI_MPI_COMM_WORLD, &BI_Iam, ierr);
   }
   *mypnum = BI_Iam;
   *nprocs = BI_Np;
}
