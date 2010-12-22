#include "Bdef.h"

int BI_BuffIsFree(BLACBUFF *bp, int Wait)
/*
 *  Check to see if buff is finished with async. operations.  If Wait != 0,
 *  wait for all async. operations to complete.
 */
{
   int i, info;
   extern BI_MPI_Status *BI_Stats;
   extern int BI_Np;


   if (!Wait)
   {
      BI_MPI_Testall(bp->nAops, bp->Aops, &i, BI_Stats, info);
      if (!i)
      {
/*
 *       If we are doing our own Packing, need to check true length of receive
 */
#ifndef MpiBuffGood
/*
 *       If we have an outstanding receive, make sure that when it
 *       completes we correctly set bp->N, if required
 */
         if (bp->N < 0)
         {
            if (bp->Aops[-bp->N-1] == BI_MPI_REQUEST_NULL)
            {
               BI_MPI_Get_count(&BI_Stats[(-bp->N-1)*BI_MPI_STATUS_SIZE],
                                BI_MPI_PACKED, &i, info);
               if (i != BI_MPI_UNDEFINED) bp->N = i;
               else BI_BlacsWarn(-1, __LINE__, __FILE__,
                                 "MPI_Get_count returned MPI_UNDEFINED.\n");
            }
         }
#endif
	 return(0);
      }
   }
   else
   {
      BI_MPI_Waitall(bp->nAops, bp->Aops, BI_Stats, info);
   }

   bp->nAops = 0;
/*
 * If we are doing our own packing, need to check true length of receive
 */
#ifndef MpiBuffGood
/*
 * If we had an outstanding receive, make sure that we correctly set bp->N,
 * if required
 */
   if (bp->N < 0)
   {
      BI_MPI_Get_count(&BI_Stats[(-bp->N-1)*BI_MPI_STATUS_SIZE],
                       BI_MPI_PACKED, &i, info);
      if (i != BI_MPI_UNDEFINED) bp->N = i;
      else BI_BlacsWarn(-1, __LINE__, __FILE__,
                        "MPI_Get_count returned MPI_UNDEFINED.\n");
   }
#endif
   return(1);
}
