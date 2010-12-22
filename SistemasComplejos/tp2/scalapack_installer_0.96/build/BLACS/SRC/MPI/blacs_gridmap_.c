#include "Bdef.h"
/* This file from mpiblacs_patch01 */

#if (INTFACE == C_CALL)
void Cblacs_gridmap(int *ConTxt, int *usermap, int ldup, int nprow0, int npcol0)
#else
F_VOID_FUNC blacs_gridmap_(int *ConTxt, int *usermap, int *ldup, int *nprow0,
                           int *npcol0)
#endif
{
   void Cblacs_pinfo(int *, int *);
   void Cblacs_get(int, int, int *);
#ifdef UseF77Mpi
   int BI_TransUserComm(MPI_Comm, int, int *);
#else
   MPI_Comm BI_TransUserComm(int, int, int *);
#endif
   MPI_Comm Cblacs2sys_handle(int);

   int info, i, j, Iam, *iptr;
   int myrow, mycol, nprow, npcol, Ng;
   BLACSCONTEXT *ctxt, **tCTxts;
   BI_MPI_Comm comm, tcomm;
   BI_MPI_Group grp, tgrp;
#if (BI_TransComm == BONEHEAD)
#ifdef UseF77Mpi
   MPI_Comm Ucomm, Ccomm=MPI_COMM_NULL;
   MPI_Group Cgrp, Cgrp2;
#else
   int Fgrp, Fgrp2, *Fcomm=NULL;
#endif
#endif
   extern BLACSCONTEXT **BI_MyContxts;
   extern BLACBUFF BI_AuxBuff;
   extern int BI_Iam, BI_Np, BI_MaxNCtxt;
   extern BI_MPI_Status *BI_Stats;

/*
 * If first call to blacs_gridmap
 */
   if (BI_MaxNCtxt == 0)
   {
      Cblacs_pinfo(&BI_Iam, &BI_Np);
      BI_AuxBuff.nAops = 0;
      BI_AuxBuff.Aops = (BI_MPI_Request*)malloc(BI_Np*sizeof(*BI_AuxBuff.Aops));
      BI_Stats = (BI_MPI_Status *) malloc(BI_Np * BI_MPI_STATUS_SIZE *
                                          sizeof(BI_MPI_Status));
#ifndef UseF77Mpi
      BI_MPI_Type_contiguous(2, BI_MPI_FLOAT, &BI_MPI_COMPLEX, info);
      BI_MPI_Type_commit(&BI_MPI_COMPLEX, info);
      BI_MPI_Type_contiguous(2, BI_MPI_DOUBLE, &BI_MPI_DOUBLE_COMPLEX, info);
      BI_MPI_Type_commit(&BI_MPI_DOUBLE_COMPLEX, info);
#endif
   }

   nprow = Mpval(nprow0);
   npcol = Mpval(npcol0);
   Ng = nprow * npcol;
   if ( (Ng > BI_Np) || (nprow < 1) || (npcol < 1) )
      BI_BlacsErr(-1, -1, "BLACS_GRIDINIT/BLACS_GRIDMAP",
                  "Illegal grid (%d x %d), #procs=%d", nprow, npcol, BI_Np);
/*
 * Form MPI communicator for scope = 'all'
 */
   if (Ng > 2) i = Ng;
   else i = 2;
   iptr = (int *) malloc(i*sizeof(int));
   for (j=0; j < npcol; j++)
   {
      for (i=0; i < nprow; i++) iptr[i*npcol+j] = usermap[j*Mpval(ldup)+i];
   }
#if (INTFACE == C_CALL)

#ifdef UseF77Mpi
   comm = BI_TransUserComm(Cblacs2sys_handle(*ConTxt), Ng, iptr);
/*
 * If we globally blocked to translate the User's communicator from C to F77,
 * go ahead and translate the new context back to F77 in case he calls blacs_get
 */
#if (BI_TransComm == BONEHEAD)
   Ucomm = Cblacs2sys_handle(*ConTxt);
   MPI_Comm_group(Ucomm, &Cgrp);            /* find input comm's group */
   MPI_Group_incl(Cgrp, Ng, iptr, &Cgrp2);  /* form new group */
   MPI_Comm_create(Ucomm, Cgrp2, &Ccomm);   /* create new comm */
   MPI_Group_free(&Cgrp);
   MPI_Group_free(&Cgrp2);
#endif
#else
#define BI_FormComm
   tcomm = Cblacs2sys_handle(*ConTxt);
#endif

#else  /* gridmap called from f77 */

#ifdef UseF77Mpi
#define BI_FormComm
   tcomm = *ConTxt;
#else
   comm = BI_TransUserComm(*ConTxt, Ng, iptr);
#if (BI_TransComm == BONEHEAD)
   Fcomm = (int *) malloc(sizeof(int));
   mpi_comm_group_(ConTxt, &Fgrp, &info);
   mpi_group_incl_(&Fgrp, &Ng, iptr, &Fgrp2, &info);
   mpi_comm_create_(ConTxt, &Fgrp2, Fcomm, &info);
   mpi_group_free_(&Fgrp2, &info);
   mpi_group_free_(&Fgrp, &info);
#endif
#endif

#endif

#ifdef BI_FormComm
   BI_MPI_Comm_group(tcomm, &grp, info);           /* find input comm's group */
   BI_MPI_Group_incl(grp, Ng, iptr, &tgrp, info);  /* form new group */
   BI_MPI_Comm_create(tcomm, tgrp, &comm, info);   /* create new comm */
   BI_MPI_Group_free(&tgrp, info);
   BI_MPI_Group_free(&grp, info);
#endif
/*
 * Weed out callers who are not participating in present grid
 */
   if (comm == BI_MPI_COMM_NULL)
   {
      *ConTxt = NOTINCONTEXT;
      free(iptr);
      return;
   }

/*
 * ==================================================
 * Get new context and add it to my array of contexts
 * ==================================================
 */
   ctxt = (BLACSCONTEXT *) malloc(sizeof(BLACSCONTEXT));
/*
 * Find free slot in my context array
 */
   for (i=0; i < BI_MaxNCtxt; i++) if (BI_MyContxts[i] == NULL) break;
/*
 * Get bigger context pointer array, if needed
 */
   if (i == BI_MaxNCtxt)
   {
      j = BI_MaxNCtxt + MAXNCTXT;
      tCTxts = (BLACSCONTEXT **) malloc(j * sizeof(*tCTxts));
      for (i=0; i < BI_MaxNCtxt; i++) tCTxts[i] = BI_MyContxts[i];
      BI_MaxNCtxt = j;
      for(j=i; j < BI_MaxNCtxt; j++) tCTxts[j] = NULL;
      if (BI_MyContxts) free(BI_MyContxts);
      BI_MyContxts = tCTxts;
   }
   BI_MyContxts[i] = ctxt;
   *ConTxt = i;

#if (BI_TransComm == BONEHEAD)
#ifdef UseF77Mpi
   ctxt->C_comm = Ccomm;
#else
   ctxt->F77_comm = Fcomm;
#endif
#endif
   ctxt->ascp.comm = comm;
   BI_MPI_Comm_dup(comm, &ctxt->pscp.comm, info); /* copy acomm for pcomm */
   BI_MPI_Comm_rank(comm, &Iam, info);            /* find my rank in new comm */
   myrow = Iam / npcol;
   mycol = Iam % npcol;

/*
 * Form MPI communicators for scope = 'row'
 */
   BI_MPI_Comm_split(comm, myrow, mycol, &ctxt->rscp.comm, info);
/*
 * Form MPI communicators for scope = 'Column'
 */
   BI_MPI_Comm_split(comm, mycol, myrow, &ctxt->cscp.comm, info);

   ctxt->rscp.Np = npcol;
   ctxt->rscp.Iam = mycol;
   ctxt->cscp.Np = nprow;
   ctxt->cscp.Iam = myrow;
   ctxt->pscp.Np = ctxt->ascp.Np = Ng;
   ctxt->pscp.Iam = ctxt->ascp.Iam = Iam;
   ctxt->Nr_bs = ctxt->Nr_co = 1;
   ctxt->Nb_bs = ctxt->Nb_co = 2;
   ctxt->TopsRepeat = ctxt->TopsCohrnt = 0;

/*
 * ===========================
 * Set up the message id stuff
 * ===========================
 */
   Cblacs_get(-1, 1, iptr);
   ctxt->pscp.MinId = ctxt->rscp.MinId = ctxt->cscp.MinId =
   ctxt->ascp.MinId = ctxt->pscp.ScpId = ctxt->rscp.ScpId =
   ctxt->cscp.ScpId = ctxt->ascp.ScpId = iptr[0];
   ctxt->pscp.MaxId = ctxt->rscp.MaxId = ctxt->cscp.MaxId =
   ctxt->ascp.MaxId = iptr[1];
   free(iptr);

}
