#include "Bdef.h"

#ifdef UseF77Mpi
int BI_TransUserComm(MPI_Comm Ucomm, int Np, int *pmap)
#else
MPI_Comm BI_TransUserComm(int Ucomm, int Np, int *pmap)
#endif
/*
 * Here I am using system-dependent internals to tranlate a fortran
 * communicator to a C communicator, or vice versa.  Since these routines are
 * not part of the standard, this routine is fragile, and may change with
 * releases.  If no translation is possible, we can convert all ranks to
 * MPI_COMM_WORLD, and thus make the translation.  However,
 * this approach makes it so all processes in MPI_COMM_WORLD must call
 * BLACS_GRIDMAP for it to complete, when the translation takes place.  I.e.,
 * if BLACS_GRIDMAP is called from C, but the F77 internals are used.
 */
{
   BI_MPI_Comm bcomm, ucomm;
   BI_MPI_Group bgrp, ugrp;
   int i;
#ifdef UseF77Mpi
   void BI_MPI_C_to_f77_trans_comm(MPI_Comm, int *);
#else
   void BI_MPI_F77_to_c_trans_comm(int F77comm, MPI_Comm *Ccomm);
#endif

#if (BI_TransComm == BONEHEAD)

   int k;
#ifdef UseF77Mpi
   MPI_Group Ugrp, Wgrp;

/*
 * Translate ranks based on user's comm to MPI_COMM_WORLD
 */
   MPI_Comm_group(Ucomm, &Ugrp);
   MPI_Comm_group(MPI_COMM_WORLD, &Wgrp);
   for (i=0; i < Np; i++)
   {
      k = pmap[i];
      MPI_Group_translate_ranks(Ugrp, 1, &k, Wgrp, &pmap[i]);
   }
   ucomm = *BI_F77_MPI_COMM_WORLD;
   MPI_Group_free(&Ugrp);
   MPI_Group_free(&Wgrp);
#else
   int Ugrp, Wgrp, ierr, one=1;

   mpi_comm_group_(&Ucomm, &Ugrp, &ierr);
   mpi_comm_group_(BI_F77_MPI_COMM_WORLD, &Wgrp, &ierr);
   for (i=0; i < Np; i++)
   {
      k = pmap[i];
      mpi_group_translate_ranks_(&Ugrp, &one, &k, &Wgrp, &pmap[i], &ierr);
   }
   ucomm = MPI_COMM_WORLD;
   mpi_group_free_(&Ugrp, &ierr);
   mpi_group_free_(&Wgrp, &ierr);
#endif

#else

#ifdef UseF77Mpi
   BI_MPI_C_to_f77_trans_comm(Ucomm, &ucomm);
#else
   BI_MPI_F77_to_c_trans_comm(Ucomm, &ucomm);
#endif

#endif

   BI_MPI_Comm_group(ucomm, &ugrp, i);
   BI_MPI_Group_incl(ugrp, Np, pmap, &bgrp, i);
   BI_MPI_Comm_create(ucomm, bgrp, &bcomm, i);
   BI_MPI_Group_free(&ugrp, i);
   BI_MPI_Group_free(&bgrp, i);

   return(bcomm);
}
