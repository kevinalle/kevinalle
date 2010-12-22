#include "Bdef.h"

#ifdef __STDC__
void BI_BlacsErr(int ConTxt, int line, char *file, char *form, ...)
#else
void BI_BlacsErr(va_alist)
va_dcl
#endif
{
#ifdef __STDC__
   void BI_BlacsAbort(int ErrNo);
#else
   void BI_BlacsAbort();
#endif
   extern int BI_Iam;
   int myrow, mycol;
   va_list argptr;
   char cline[100];
   BLACSCONTEXT *ctxt;

#ifdef __STDC__
   va_start(argptr, form);
#else
   char *file, *form;
   int ConTxt, line;

   va_start(argptr);
   ConTxt = va_arg(argptr, int);
   line = va_arg(argptr, int);
   file = va_arg(argptr, char *);
   form = va_arg(argptr, char *);
#endif
   vsprintf(cline, form, argptr);
   va_end(argptr);

   if (ConTxt > -1)
   {
      MGetConTxt(ConTxt, ctxt);
      myrow = ctxt->cscp.Iam;
      mycol = ctxt->rscp.Iam;
   }
   else myrow = mycol = -1;

   fprintf(stderr,
"BLACS ERROR '%s'\nfrom {%d,%d}, pnum=%d, Contxt=%d, on line %d of file '%s'.\n\n",
           cline, myrow, mycol, BI_Iam, ConTxt, line, file);

   BI_BlacsAbort(1);
}
