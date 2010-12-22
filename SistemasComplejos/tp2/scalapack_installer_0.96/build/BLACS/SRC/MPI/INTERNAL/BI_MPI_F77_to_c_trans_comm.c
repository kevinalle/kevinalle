#include "Bdef.h"
/* This file from mpiblacs_patch01, hacked by author to add MPI2 support */

void BI_MPI_F77_to_c_trans_comm(int F77comm, MPI_Comm *Ccomm)
/*
 * Here I am using system-dependent internals to tranlate a C
 * communicator to a F77 communicator, or vice versa.  Since these routines are
 * not part of the standard, this routine is fragile, and may change with
 * releases.  If no translation is possible, we can convert all ranks to
 * MPI_COMM_WORLD, and thus make the translation.  However,
 * this approach makes it so all processes in MPI_COMM_WORLD must call
 * this routine for it to complete.
 */
{

/*
 * If your packages supports MPI 2.0's translation mechanisms
 */
#if (BI_TransComm == USEMPI2)
   *Ccomm = MPI_Comm_f2c(F77comm);
#endif

/*
 * If the MPI we're using is based on MPICH, can use MPICH's internal
 * translation * routines (I found these routines in MPICH version 1.0.9,
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

   *Ccomm = (MPI_Comm) MPIR_ToPointer(F77comm);

#endif

/*
 * Some systems may implement it so fortran and C handles are the same.  If so,
 * this guy does the obvious replacement . . .
 */
#if (BI_TransComm == CSAMEF77)

   *Ccomm = (MPI_Comm) F77comm;

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
   int Fgrp, Wgrp, ierr, one=1;
   MPI_Group wgrp, cgrp;

   mpi_comm_size_(&F77comm, &Np, &ierr);
   pmap = (int *) malloc(Np * sizeof(int));
   mpi_comm_group_(&F77comm, &Fgrp, &ierr);
   mpi_comm_group_(BI_F77_MPI_COMM_WORLD, &Wgrp, &ierr);
   for (i=0; i < Np; i++)
   {
      mpi_group_translate_ranks_(&Fgrp, &one, &i, &Wgrp, &pmap[i], &ierr);
   }
   mpi_group_free_(&Wgrp, &ierr);
   mpi_group_free_(&Fgrp, &ierr);

   MPI_Comm_group(MPI_COMM_WORLD, &wgrp);
   MPI_Group_incl(wgrp, Np, pmap, &cgrp);
   MPI_Group_free(&wgrp);
   free(pmap);
   MPI_Comm_create(MPI_COMM_WORLD, cgrp, Ccomm);
   MPI_Group_free(&cgrp);

#endif
}
