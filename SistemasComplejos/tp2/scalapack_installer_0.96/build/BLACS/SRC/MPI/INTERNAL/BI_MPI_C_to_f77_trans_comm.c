#include "Bdef.h"
/* This file from mpiblacs_patch01 */
/* This file subsequently hacked by author to add support for MPI2.0 translation
   mechanisms */

void BI_MPI_C_to_f77_trans_comm(MPI_Comm Ccomm, int *F77comm)
/*
 * Here I am using system-dependent internals to tranlate a C communicator
 * to a f77 communicator.  Since these routines are
 * not part of the standard, this routine is fragile, and may change with
 * releases.  If no translation is possible, we can convert all ranks to
 * MPI_COMM_WORLD, and thus make the translation.  However,
 * this approach makes it so all processes in MPI_COMM_WORLD must call
 * this routine it to complete.
 */
{

/*
 * If your packages supports MPI 2.0's translation mechanisms
 */
#if (BI_TransComm == USEMPI2)
   *F77comm = MPI_Comm_c2f(Ccomm);
#endif

/*
 * If the MPI we're using is based on MPICH, can use MPICH's internal
 * translation routines (I found these routines in MPICH version 1.0.9,
 * June, 1995).
 */
#if (BI_TransComm == USEMPICH)

#ifdef POINTER_64_BITS
extern void *MPIR_ToPointer();
extern int MPIR_FromPointer();
extern void MPIR_RmPointer();
#else
#define MPIR_ToPointer(a) (a)
#define MPIR_FromPointer(a) (int)(a)
#define MPIR_RmPointer(a)
#endif

   *F77comm = MPIR_FromPointer(Ccomm);

#endif

/*
 * Some systems may implement it so fortran and C handles are the same.  If so,
 * this guy does the obvious replacement . . .
 */
#if (BI_TransComm == CSAMEF77)

   *F77comm = (int) Ccomm;

#endif

/*
 * If we don't know a clever way to perform translation, we do the boneheaded
 * thing, and translate all ranks to MPI_COMM_WORLD (which we assume means
 * the same thing in both languages, i.e. rank 1 in C's MPI_COMM_WORLD is
 * rank 1 in F77's MPI_COMM_WORLD).  We form our new comm based on
 * MPI_COMM_WORLD, which means that all processes in MPI_COMM_WORLD must
 * call this routine . . .
 */
#if (BI_TransComm == BONEHEAD)

   int i, Np, *pmap;
   int bgrp, ugrp;
   MPI_Group Ugrp, Wgrp;

/*
 * Translate ranks based on user's comm to MPI_COMM_WORLD
 */
   MPI_Comm_size(Ccomm, &Np);
   MPI_Comm_group(Ccomm, &Ugrp);
   MPI_Comm_group(MPI_COMM_WORLD, &Wgrp);
   pmap = (int *) malloc(Np * sizeof(int));
   for (i=0; i < Np; i++)
   {
      MPI_Group_translate_ranks(Ugrp, 1, &i, Wgrp, &pmap[i]);
   }
   MPI_Group_free(&Wgrp);
   MPI_Group_free(&Ugrp);

   mpi_comm_group_(BI_F77_MPI_COMM_WORLD, &ugrp, &i);
   mpi_group_incl_(&ugrp, &Np, pmap, &bgrp, &i);
   mpi_group_free_(&ugrp, &i);
   free(pmap);
   mpi_comm_create_(BI_F77_MPI_COMM_WORLD, &bgrp, F77comm, &i);
   mpi_group_free_(&bgrp, &i);

#endif

}
