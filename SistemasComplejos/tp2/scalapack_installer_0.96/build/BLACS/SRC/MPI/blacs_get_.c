#include "Bdef.h"
#if (INTFACE == C_CALL)
void Cblacs_get(int ConTxt, int what, int *val)
#else
F_VOID_FUNC blacs_get_(int *ConTxt, int *what, int *val)
#endif
{
   int Csys2blacs_handle(MPI_Comm);
   int ierr, *iptr;
#ifdef UseF77Mpi
   MPI_Comm comm;
#else
   int comm;
#endif
   BLACSCONTEXT *ctxt;

   switch( Mpval(what) )
   {
   case SGET_SYSCONTXT:
      if (BI_F77_MPI_COMM_WORLD == NULL) Cblacs_pinfo(val, &ierr);
#if (INTFACE == C_CALL)
      *val = Csys2blacs_handle(MPI_COMM_WORLD);
#else
      *val = *BI_F77_MPI_COMM_WORLD;
#endif
      break;
   case SGET_MSGIDS:
      if (BI_F77_MPI_COMM_WORLD == NULL) Cblacs_pinfo(val, &val[1]);
      iptr = &val[1];
      BI_MPI_Attr_get(BI_MPI_COMM_WORLD, BI_MPI_TAG_UB, (BVOID **) &iptr,
                      val, ierr);
      val[0] = 0;
      val[1] = *iptr;
      break;
   case SGET_DEBUGLVL:
      *val = BlacsDebugLvl;
      break;
   case SGET_BLACSCONTXT:
      MGetConTxt(Mpval(ConTxt), ctxt);
#if (INTFACE == C_CALL)

#ifdef UseF77Mpi

#if (BI_TransComm == BONEHEAD)
   if (ctxt->C_comm == MPI_COMM_NULL)
   {
      BI_MPI_F77_to_c_trans_comm(ctxt->pscp.comm, &ctxt->C_comm);
   }
   *val = Csys2blacs_handle(ctxt->C_comm);
#else
   BI_MPI_F77_to_c_trans_comm(ctxt->pscp.comm, &comm);
   *val = Csys2blacs_handle(comm);
#endif

#else  /* we are returning a C handle, and using the C MPI interface */
   *val = Csys2blacs_handle(ctxt->pscp.comm);
#endif

#else  /* if user called the fortran interface to the BLACS */

#ifdef UseF77Mpi

   *val = ctxt->pscp.comm;

#else  /* User called F77 interface, but we're using C interface MPI */

#if (BI_TransComm == BONEHEAD)
   if (ctxt->F77_comm == NULL)
   {
      ctxt->F77_comm = (int *) malloc(sizeof(int));
      BI_MPI_C_to_f77_trans_comm(ctxt->pscp.comm, ctxt->F77_comm);
   }
   *val = *ctxt->F77_comm;
#else
   BI_MPI_C_to_f77_trans_comm(ctxt->pscp.comm, &comm);
   *val = comm;
#endif

#endif

#endif
      break;
   case SGET_NR_BS:
      MGetConTxt(Mpval(ConTxt), ctxt);
      *val = ctxt->Nr_bs;
      break;
   case SGET_NB_BS:
      MGetConTxt(Mpval(ConTxt), ctxt);
      *val = ctxt->Nb_bs - 1;
      break;
   case SGET_NR_CO:
      MGetConTxt(Mpval(ConTxt), ctxt);
      *val = ctxt->Nr_co;
      break;
   case SGET_NB_CO:
      MGetConTxt(Mpval(ConTxt), ctxt);
      *val = ctxt->Nb_co - 1;
      break;
   case SGET_TOPSREPEAT:
      MGetConTxt(Mpval(ConTxt), ctxt);
      *val = ctxt->TopsRepeat;
      break;
   case SGET_TOPSCOHRNT:
      MGetConTxt(Mpval(ConTxt), ctxt);
      *val = ctxt->TopsCohrnt;
      break;
   default:
      BI_BlacsWarn(Mpval(ConTxt), __LINE__, __FILE__, "Unknown WHAT (%d)",
                Mpval(what));
   }
}
