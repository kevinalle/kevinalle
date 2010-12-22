#include "Bdef.h"

#ifdef __STDC__
void BI_BlacsWarn(int ConTxt, int line, char *file, char *form, ...)
#else
void BI_BlacsWarn(va_alist)
va_dcl
#endif
{
   extern int BI_Iam;
   extern BLACSCONTEXT **BI_MyContxts;
   int myrow, mycol;
   va_list argptr;
   char cline[100];

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
      myrow = BI_MyContxts[ConTxt]->cscp.Iam;
      mycol = BI_MyContxts[ConTxt]->rscp.Iam;
   }
   else myrow = mycol = -1;

   fprintf(stderr,
"BLACS WARNING '%s'\nfrom {%d,%d}, pnum=%d, Contxt=%d, on line %d of file '%s'.\n\n",
           cline, myrow, mycol, BI_Iam, ConTxt, line, file);
}
