#include "Bdef.h"
/*
 * Define global variables
 */
int BI_MaxNCtxt=0;		  /* Number of context pointers allocated */
int BI_MaxNSysCtxt=0;             /* Number of system ctxt ptrs allocated */
int BI_Iam, BI_Np=(-1);	          /* My pnum, and # of procs in system */
BLACBUFF *BI_ReadyB=NULL;         /* buffer that is ready for use */
BLACBUFF *BI_ActiveQ=NULL;        /* pointer to start of active buffer queue */
BLACBUFF BI_AuxBuff;
BLACSCONTEXT **BI_MyContxts=NULL; /* Array of pointers to my contexts */
BI_MPI_Comm *BI_SysContxts=NULL;
#ifdef UseF77Mpi
int *BI_F77_MPI_CONSTANTS;        /* Array of fortran MPI constants */
#else
BI_MPI_Datatype BI_MPI_COMPLEX, BI_MPI_DOUBLE_COMPLEX;
#endif
int *BI_F77_MPI_COMM_WORLD=NULL;
BI_MPI_Status *BI_Stats=NULL;
