#include "Bdef.h"

#if (INTFACE == C_CALL)
void Cblacs_barrier(int ConTxt, char *scope)
#else
F_VOID_FUNC blacs_barrier_(int *ConTxt, F_CHAR scope)
#endif
{
   char tscope;
   int ierr;
   BLACSCONTEXT *ctxt;

   tscope = F2C_CharTrans(scope);
   tscope = Mlowcase(tscope);
   MGetConTxt(Mpval(ConTxt), ctxt);
   switch(tscope)
   {
   case 'r':
      BI_MPI_Barrier(ctxt->rscp.comm, ierr);
      break;
   case 'c':
      BI_MPI_Barrier(ctxt->cscp.comm, ierr);
      break;
   case 'a':
      BI_MPI_Barrier(ctxt->ascp.comm, ierr);
      break;
   }
}
