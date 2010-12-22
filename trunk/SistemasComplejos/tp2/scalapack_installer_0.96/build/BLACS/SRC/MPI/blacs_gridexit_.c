#include "Bdef.h"

#if (INTFACE == C_CALL)
void Cblacs_gridexit(int ConTxt)
#else
F_VOID_FUNC blacs_gridexit_(int *ConTxt)
#endif
{
   int i;
   BLACSCONTEXT *ctxt;
   extern int BI_MaxNCtxt;
   extern BLACSCONTEXT **BI_MyContxts;

   if ( (Mpval(ConTxt) < 0) || (Mpval(ConTxt) >= BI_MaxNCtxt) )
      BI_BlacsErr(Mpval(ConTxt), __LINE__, __FILE__,
 	          "Trying to exit non-existent context");

   if (BI_MyContxts[Mpval(ConTxt)] == NULL)
      BI_BlacsErr(Mpval(ConTxt), __LINE__, __FILE__,
	          "Trying to exit an already freed context");

   MGetConTxt(Mpval(ConTxt), ctxt);
/*
 * Destroy context
 */
   BI_MPI_Comm_free(&ctxt->pscp.comm, i);
   BI_MPI_Comm_free(&ctxt->ascp.comm, i);
   BI_MPI_Comm_free(&ctxt->rscp.comm, i);
   BI_MPI_Comm_free(&ctxt->cscp.comm, i);
#if (BI_TransComm == BONEHEAD)
#ifdef UseF77Mpi
   if (ctxt->C_comm != MPI_COMM_NULL) MPI_Comm_free(&ctxt->C_comm);
#else
   if (ctxt->F77_comm != NULL)
   {
      mpi_comm_free_(ctxt->F77_comm, &i);
      free(ctxt->F77_comm);
      ctxt->F77_comm = NULL;
   }
#endif
#endif
   free(ctxt);
   BI_MyContxts[Mpval(ConTxt)] = NULL;
}
