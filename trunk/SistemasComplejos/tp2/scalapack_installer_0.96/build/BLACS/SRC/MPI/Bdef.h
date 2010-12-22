#ifndef BDEF_H
#define BDEF_H 1
/* This file from mpiblacs_patch01 */

/*
 * Include the system dependant and user defined stuff
 */
#include "Bconfig.h"
#ifdef CRAY
#define float double
#include <fortran.h>
#endif
/*
 * ========================================================================
 *                     TYPEDEF'S USED IN THE BLACS
 * ========================================================================
 */

/*
 * ---------------------------------------------------------------------------
 * Define MPI's data types differently depending on whether we are using MPI's
 * fortran or C interface
 * ---------------------------------------------------------------------------
 */
#ifdef UseF77Mpi

#define BI_MPI_Aint     int
#define BI_MPI_Comm     int
#define BI_MPI_Datatype int
#define BI_MPI_Group    int
#define BI_MPI_Op       int
#define BI_MPI_Request  int
#define BI_MPI_Status   int

#else

#define BI_MPI_Aint     MPI_Aint
#define BI_MPI_Comm     MPI_Comm
#define BI_MPI_Datatype MPI_Datatype
#define BI_MPI_Group    MPI_Group
#define BI_MPI_Op       MPI_Op
#define BI_MPI_Request  MPI_Request
#define BI_MPI_Status   MPI_Status

#endif


/*
 * Data type defining a scope for the BLACS
 */
typedef struct bLaCsScOpE BLACSSCOPE;
struct bLaCsScOpE
{
   BI_MPI_Comm comm;
   int ScpId, MaxId, MinId;
   int Np, Iam;
};
/*
 * Data type defining a context for the BLACS
 */
typedef struct bLaCsCoNtExT BLACSCONTEXT;
struct bLaCsCoNtExT
{
   BLACSSCOPE rscp, cscp, ascp, pscp; /* row, column, all, and pt2pt scopes */
   BLACSSCOPE *scp;                   /* pointer to present scope */
#if (BI_TransComm == BONEHEAD)
#ifdef UseF77Mpi
   MPI_Comm C_comm;
#else
   int *F77_comm;
#endif
#endif
   int TopsRepeat;                    /* Use only repeatable topologies? */
   int TopsCohrnt;                    /* Use only coherent topologies? */
   int Nb_bs, Nr_bs;           /* for bcast general tree and multiring tops */
   int Nb_co, Nr_co;           /* for combine general tree and multiring tops */
};

/*
 *  Define the fortran 77 data types COMPLEX*8 (SCOMPLEX)
 *  and COMPLEX*16 (DCOMPLEX).
 */
typedef struct {double r, i;} DCOMPLEX;
typedef struct {float r, i;} SCOMPLEX;

/*
 *  These variables will be defined to be MPI datatypes for complex and double
 *  complex if we are using the C interface to MPI.  If we use the fortran
 *  interface, we need to declare the contants array.  I'm too lazy to declare
 *  these guys external in every file that needs them.
 */
#ifndef GlobalVars

#ifdef UseF77Mpi
   extern int *BI_F77_MPI_CONSTANTS, *BI_F77_MPI_COMM_WORLD;
#else
   extern BI_MPI_Datatype BI_MPI_COMPLEX, BI_MPI_DOUBLE_COMPLEX;
   extern int *BI_F77_MPI_COMM_WORLD;
#endif

#endif

/*
 *  Definition of buffer type for BLACS' asynchronous operations
 */
typedef struct bLaCbUfF BLACBUFF;
struct bLaCbUfF
{
   char *Buff;             /* send/recv buffer */
   int Len;                /* length of buffer in bytes */
   int nAops;              /* number of asynchronous operations out of buff */
   BI_MPI_Request *Aops;   /* list of async. operations out of buff */
   BI_MPI_Datatype dtype;  /* data type of buffer */
   int N;                  /* number of elements of data type in buff */
   BLACBUFF *prev, *next;  /* pointer to the other BLACBUFF in queue */
};

/*
 * Pointer to the combine's vector-vector functions
 */
#ifdef __STDC__
typedef void (*VVFUNPTR)(int, char *, char *);
typedef void (*SDRVPTR)(BLACSCONTEXT *, int, int, BLACBUFF *);
#else
typedef void (*VVFUNPTR)();
typedef void (*SDRVPTR)();
#endif

/*
 * ========================================================================
 *                             MACRO CONSTANTS
 * ========================================================================
 */
/*
 * ----------------------------------------------
 * Define MPI's constants for fortran77 interface
 * ----------------------------------------------
 */
#ifdef UseF77Mpi

/*
 *  return codes
 */
#define BI_MPI_SUCCESS                 BI_F77_MPI_CONSTANTS[0]
#define BI_MPI_ERR_UNKNOWN             BI_F77_MPI_CONSTANTS[1]
#define BI_MPI_ERR_OTHER               BI_F77_MPI_CONSTANTS[2]
#define BI_MPI_ERR_INTERN              BI_F77_MPI_CONSTANTS[3]

/*
 *  Assorted constants
 */
#define BI_MPI_ANY_SOURCE              BI_F77_MPI_CONSTANTS[4]
#define BI_MPI_UNDEFINED               BI_F77_MPI_CONSTANTS[5]

/*
 *  Status size and reserved index values
 */
#define BI_MPI_STATUS_SIZE             BI_F77_MPI_CONSTANTS[6]
#define BI_MPI_SOURCE                  BI_F77_MPI_CONSTANTS[7]
#define BI_MPI_TAG                     BI_F77_MPI_CONSTANTS[8]

/*
 *  Elementary datatypes
 */
#define BI_MPI_INT                     BI_F77_MPI_CONSTANTS[9]
#define BI_MPI_FLOAT                   BI_F77_MPI_CONSTANTS[10]
#define BI_MPI_DOUBLE                  BI_F77_MPI_CONSTANTS[11]
#define BI_MPI_COMPLEX                 BI_F77_MPI_CONSTANTS[12]
#define BI_MPI_DOUBLE_COMPLEX          BI_F77_MPI_CONSTANTS[13]
#define BI_MPI_PACKED                  BI_F77_MPI_CONSTANTS[14]
#define BI_MPI_BYTE                    BI_F77_MPI_CONSTANTS[15]

/*
 * Reserved communicators
 */
#define BI_MPI_COMM_WORLD              BI_F77_MPI_CONSTANTS[16]
#define BI_MPI_COMM_NULL               BI_F77_MPI_CONSTANTS[17]

/*
 * Environmental inquiry keys
 */
#define BI_MPI_TAG_UB BI_F77_MPI_CONSTANTS[18]

/*
 * Collective operations
 */
#define BI_MPI_MAX                     BI_F77_MPI_CONSTANTS[19]
#define BI_MPI_MIN                     BI_F77_MPI_CONSTANTS[20]
#define BI_MPI_SUM                     BI_F77_MPI_CONSTANTS[21]

/*
 * NULL handles
 */
#define BI_MPI_REQUEST_NULL          BI_F77_MPI_CONSTANTS[22]

/*
 * Data types to use in the combine operations
 */
#define BI_DistType                  int
#define BI_MpiDistType               BI_MPI_INT

/*
 * ====================================
 * Define MPI constants for C interface
 * ====================================
 */
#else
/*
 *  return codes
 */
#define BI_MPI_SUCCESS               MPI_SUCCESS
#define BI_MPI_ERR_UNKNOWN           MPI_ERR_UNKNOWN
#define BI_MPI_ERR_OTHER             MPI_ERR_OTHER
#define BI_MPI_ERR_INTERN            MPI_ERR_INTERN

/*
 *  Assorted constants
 */
#define BI_MPI_ANY_SOURCE            MPI_ANY_SOURCE
#define BI_MPI_UNDEFINED             MPI_UNDEFINED

/*
 *  Status size and reserved index values
 */
#define BI_MPI_STATUS_SIZE           1

/*
 *  Elementary datatypes
 */
#define BI_MPI_SHORT                 MPI_SHORT
#define BI_MPI_INT                   MPI_INT
#define BI_MPI_FLOAT                 MPI_FLOAT
#define BI_MPI_DOUBLE                MPI_DOUBLE
#define BI_MPI_PACKED                MPI_PACKED
#define BI_MPI_BYTE                  MPI_BYTE

/*
 * Reserved communicators
 */
#define BI_MPI_COMM_WORLD            MPI_COMM_WORLD
#define BI_MPI_COMM_NULL             MPI_COMM_NULL

/*
 * Environmental inquiry keys
 */
#define BI_MPI_TAG_UB                MPI_TAG_UB

/*
 * Collective operations
 */
#define BI_MPI_MAX                   MPI_MAX
#define BI_MPI_MIN                   MPI_MIN
#define BI_MPI_SUM                   MPI_SUM

/*
 * NULL handles
 */
#define BI_MPI_REQUEST_NULL          MPI_REQUEST_NULL
/*
 * Data types to use in the combine operations
 */
#define BI_DistType                  unsigned short
#define BI_MpiDistType               MPI_UNSIGNED_SHORT

#endif

#define BUFFALIGN    8      /* force all buffers to 8 byte alignment */
#define BANYNODE     BI_MPI_ANY_SOURCE
#define PT2PTID      9976   /* TAG used for point to point */
#define NOTINCONTEXT -1  /* Indicates node called gridmap, but not in grid */
#define MAXNCTXT 10      /* initial guess at max # of contexts */
#define MAXNSYSCTXT 10   /* initial guess at max # of system context */
#define AOPDONE BI_MPI_REQUEST_NULL
#define BUFWAIT 120      /* Length of time to wait for emergency buff */

/*
 * Error codes
 */
#define NORV 1          /* No receiver (only 1 proc in scoped op) */
#define NPOW2 2         /* Number of procs is not a power of 2 */
#define BADSCP 3        /* Scope not row, column or all */

/*
 * Data types
 */
#define INTEGER   3
#define SINGLE    4
#define DOUBLE    6
#define COMPLEX8  5
#define COMPLEX16 7

#define FULLCON 0      /* top is fully connected */

/*
 * Routine types
 */
#define RT_SD    1
#define RT_RV    2
#define RT_BS    3
#define RT_BR    4
#define RT_COMB  5

/*
 * Legal WHAT values for BLACS_SET
 */
#define SGET_SYSCONTXT    0
#define SGET_MSGIDS       1
#define SGET_DEBUGLVL     2
#define SGET_BLACSCONTXT 10
#define SGET_NR_BS       11
#define SGET_NB_BS       12
#define SGET_NR_CO       13
#define SGET_NB_CO       14
#define SGET_TOPSREPEAT  15
#define SGET_TOPSCOHRNT  16

/*
 * These are prototypes for error and warning functions -- I don't want
 * to prototype them in each routine.
 */
#ifdef __STDC__
void BI_BlacsWarn(int ConTxt, int line, char *file, char *form, ...);
void BI_BlacsErr(int ConTxt, int line, char *file, char *form, ...);
int BI_ContxtNum(BLACSCONTEXT *ctxt);
#else
void BI_BlacsWarn();
void BI_BlacsErr();
int BI_ContxtNum();
#endif

/*
 * If we've got an ANSI standard C compiler, we can use void pointers,
 * otherwise use char pointers
 */
#ifdef __STDC__
#define BVOID void
#else
#define BVOID char
#endif

/*
 * ========================================================================
 *     PREPROCESSOR MACRO FUNCTIONS USED FOR OPTIMIZATION & CONVENIENCE
 * ========================================================================
 */

#define Mlowcase(C) ( ((C) > 64 && (C) < 91) ? (C) | 32 : (C) )

/*
 * Slightly modified gridinfo substitute
 */
#define Mgridinfo(ctxt, Ng0, nprow0, npcol0, myrow0, mycol0)\
{\
   (Ng0) = (ctxt)->ascp.Np;\
   (nprow0) = (ctxt)->cscp.Np;\
   (npcol0) = (ctxt)->rscp.Np;\
   (myrow0) = (ctxt)->cscp.Iam;\
   (mycol0) = (ctxt)->rscp.Iam;\
}

/*
 * These routines return coordinates based on nodes number, or node number
 * based on coordinates.  Those routines with v after the M return virtual
 * nodes numbers (i.e., in respect to the grid, not physical node numbers)
 * based on grid coordinates, or grid coordinates based on virtual node numbers.
 */
#define Mpcoord(ctxt, node, prow, pcol)\
{\
   (prow) = (node) / (ctxt)->rscp.Np;\
   (pcol) = (node) % (ctxt)->rscp.Np;\
}
#define Mvpcoord(ctxt, node, prow, pcol) \
        Mpcoord((ctxt), (node), (prow), (pcol));

#define Mkpnum(ctxt, prow, pcol)  ( (prow)*(ctxt)->rscp.Np+(pcol) )
#define Mvkpnum(ctxt, prow, pcol) ( (prow)*(ctxt)->rscp.Np+(pcol) )

/*
 * This macro returns scoped message ID's.
 */
#define Mscopeid(ctxt) (ctxt)->scp->ScpId; \
   if (++(ctxt)->scp->ScpId == (ctxt)->scp->MaxId) \
      (ctxt)->scp->ScpId = (ctxt)->scp->MinId;

/*
 *  Get context, and check for validity if debug level is high
 */
#if (BlacsDebugLvl > 0)
#define MGetConTxt(Context, ctxtptr)\
{\
   extern BLACSCONTEXT **BI_MyContxts;\
   extern int BI_MaxNCtxt;\
   if ( ((Context) >= BI_MaxNCtxt) || ((Context) < 0) )\
      BI_BlacsErr(-1, __LINE__, __FILE__, "Invalid context handle: %d",\
                  (Context));\
   else if (BI_MyContxts[(Context)] == NULL)\
      BI_BlacsErr(-1, __LINE__, __FILE__, "Invalid context, handle=%d",\
                  (Context));\
   (ctxtptr) = BI_MyContxts[(Context)];\
}
#else
#define MGetConTxt(Context, ctxtptr)\
{\
   extern BLACSCONTEXT **BI_MyContxts;\
   (ctxtptr) = BI_MyContxts[(Context)];\
}
#endif
/*
 * This macro handles MPI errors
 */
#if(BlacsDebugLvl > 0)
#define Mmpierror(ierr, rout, ctxt, line, file) \
{ \
   if ( (ierr) != BI_MPI_SUCCESS )\
      BI_BlacsErr(BI_ContxtNum((ctxt)), (line), (file), \
                  "MPI error %d on call to %s", (ierr), (rout)); \
}
#else
#define Mmpierror(ierr, rout, ctxt, line, file)
#endif
/*
 * A small macro useful for debugging
 */
#define ErrPrint \
{ \
   extern int BI_Iam; \
   fprintf(stderr, "%d: line %d of file %s\n", BI_Iam, __LINE__, __FILE__); \
}

/*
 * These macros allow for the funky function declarations and character handling
 * needed on the CRAY to have a C routine callable from fortran
 */
#define F_VOID_FUNC void
#define F_INT_FUNC  int
#define F_DOUBLE_FUNC double

#if (INTFACE == C_CALL)

#define F2C_CharTrans(c) *(c)

#else

#ifdef CRAY
#define F2C_CharTrans(c) *( _fcdtocp((c)) )
#define F_CHAR      _fcd
#else
#define F2C_CharTrans(c) *(c)
#define F_CHAR      char *
#endif

#endif

/*
 *  These macros allow for accessing values and addresses of parameters, which
 *  will be pointers if we're using fortran, and values if we're using C.
 */
#if (INTFACE == C_CALL)
#define Mpval(para) (para)
#define Mpaddress(para) (&(para))
#define Mwalltime Cdwalltime00
#else
#define Mpval(para) (*(para))
#define Mpaddress(para) (para)
#define Mwalltime dwalltime00_
#endif

/*
 * Real and complex absolute values
 */
#define Rabs(x) ( (x) < 0 ? (x) * -1 : (x) )
#define Cabs(z) ( (((z).i) < 0 ? ((z).i) * -1 : ((z).i)) + (((z).r) < 0 ? ((z).r) * -1 : ((z).r)) )

/*
 * Figures the length of packed trapezoidal matrix
 */
#define trsize(diag, m, n, bytes, length)\
{\
   if ( (diag) == 'u' ) (length) = 1;\
   else (length) = 0;\
   if ( (m) > (n) )\
      (length) = ( (n) * ( (m) - (n) ) + ( (n)*(n) ) - ( (n)*(n) )/2 +\
                   (n)/2 - (n) * (length) ) * (bytes);\
   else\
      (length) = ( (m) * ( (n) - (m) ) + ( (m)*(m) ) - ( (m)*(m) )/2 +\
                   (m)/2 - (m) * (length) ) * (bytes);\
}

/*
 * These macros call the correct packing/unpacking routines
 */
#define BI_cmvcopy(m, n, A, lda, buff) \
        BI_smvcopy(2*(m), (n), (float *) (A), 2*(lda), (float *) (buff))
#define BI_cvmcopy(m, n, A, lda, buff) \
        BI_svmcopy(2*(m), (n), (float *) (A), 2*(lda), (float *) (buff))
#define BI_zmvcopy(m, n, A, lda, buff) \
        BI_dmvcopy(2*(m), (n), (double *) (A), 2*(lda), (double *) (buff))
#define BI_zvmcopy(m, n, A, lda, buff) \
        BI_dvmcopy(2*(m), (n), (double *) (A), 2*(lda), (double *) (buff))

#if (F77_CALL_C == ADD_)
/*
 * These defines set up the naming scheme required to have a fortran 77
 * routine call a C routine (which is what the BLACS are written in).
 * No redefinition necessary to have following Fortran to C interface:
 *           FORTRAN CALL               C DECLARATION
 *           call dgebs2d(...)          void dgebs2d_(...)
 */
#endif

#if (F77_CALL_C == NOCHANGE)
/*
 * These defines set up the naming scheme required to have a fortran 77
 * routine call a C routine (which is what the BLACS are written in)
 * for the following Fortran to C interface:
 *           FORTRAN CALL               C DECLARATION
 *           call dgebs2d(...)          void dgebs2d(...)
 */

/*
 * Support routines
 */
#define blacs_pinfo_                   blacs_pinfo
#define blacs_setup_                   blacs_setup
#define setpvmtids_                    setpvmtids
#define blacs_set_                     blacs_set
#define blacs_get_                     blacs_get
#define blacs_gridinit_                blacs_gridinit
#define blacs_gridmap_                 blacs_gridmap
#define ksendid_                       ksendid
#define krecvid_                       krecvid
#define kbsid_                         kbsid
#define kbrid_                         kbrid
#define blacs_freebuff_                blacs_freebuff
#define blacs_gridexit_                blacs_gridexit
#define blacs_abort_                   blacs_abort
#define blacs_exit_                    blacs_exit
#define blacs_gridinfo_                blacs_gridinfo
#define blacs_pnum_                    blacs_pnum
#define blacs_pcoord_                  blacs_pcoord
#define dcputime00_                    dcputime00
#define dwalltime00_                   dwalltime00
#define blacs_barrier_                 blacs_barrier

/*
 * Main, type dependent, routines
 */
#define igesd2d_   igesd2d
#define igerv2d_   igerv2d
#define igebs2d_   igebs2d
#define igebr2d_   igebr2d
#define itrsd2d_   itrsd2d
#define itrrv2d_   itrrv2d
#define itrbs2d_   itrbs2d
#define itrbr2d_   itrbr2d
#define igsum2d_   igsum2d
#define igamx2d_   igamx2d
#define igamn2d_   igamn2d
#define sgesd2d_   sgesd2d
#define sgerv2d_   sgerv2d
#define sgebs2d_   sgebs2d
#define sgebr2d_   sgebr2d
#define strsd2d_   strsd2d
#define strrv2d_   strrv2d
#define strbs2d_   strbs2d
#define strbr2d_   strbr2d
#define sgsum2d_   sgsum2d
#define sgamx2d_   sgamx2d
#define sgamn2d_   sgamn2d
#define dgesd2d_   dgesd2d
#define dgerv2d_   dgerv2d
#define dgebs2d_   dgebs2d
#define dgebr2d_   dgebr2d
#define dtrsd2d_   dtrsd2d
#define dtrrv2d_   dtrrv2d
#define dtrbs2d_   dtrbs2d
#define dtrbr2d_   dtrbr2d
#define dgsum2d_   dgsum2d
#define dgamx2d_   dgamx2d
#define dgamn2d_   dgamn2d
#define cgesd2d_   cgesd2d
#define cgerv2d_   cgerv2d
#define cgebs2d_   cgebs2d
#define cgebr2d_   cgebr2d
#define ctrsd2d_   ctrsd2d
#define ctrrv2d_   ctrrv2d
#define ctrbs2d_   ctrbs2d
#define ctrbr2d_   ctrbr2d
#define cgsum2d_   cgsum2d
#define cgamx2d_   cgamx2d
#define cgamn2d_   cgamn2d
#define zgesd2d_   zgesd2d
#define zgerv2d_   zgerv2d
#define zgebs2d_   zgebs2d
#define zgebr2d_   zgebr2d
#define ztrsd2d_   ztrsd2d
#define ztrrv2d_   ztrrv2d
#define ztrbs2d_   ztrbs2d
#define ztrbr2d_   ztrbr2d
#define zgsum2d_   zgsum2d
#define zgamx2d_   zgamx2d
#define zgamn2d_   zgamn2d

/*
 * If we are using the fortran interface to MPI, need to redefine the names
 */
#ifdef UseF77Mpi
#define mpi_abort_                     mpi_abort
#define mpi_allreduce_                 mpi_allreduce
#define bi_f77_mpi_attr_get_           bi_f77_mpi_attr_get
#define mpi_barrier_                   mpi_barrier
#define mpi_bcast_                     mpi_bcast
#define mpi_comm_create_               mpi_comm_create
#define mpi_comm_dup_                  mpi_comm_dup
#define mpi_comm_free_                 mpi_comm_free
#define mpi_comm_group_                mpi_comm_group
#define mpi_comm_rank_                 mpi_comm_rank
#define mpi_comm_size_                 mpi_comm_size
#define mpi_comm_split_                mpi_comm_split
#define mpi_error_class_               mpi_error_class
#define mpi_finalize_                  mpi_finalize
#define mpi_get_count_                 mpi_get_count
#define mpi_group_incl_                mpi_group_incl
#define mpi_group_free_                mpi_group_free
#define mpi_init_                      mpi_init
#define bi_f77_mpi_initialized_        bi_f77_mpi_initialized
#define mpi_irecv_                     mpi_irecv
#define mpi_isend_                     mpi_isend
#define bi_f77_mpi_op_create_          bi_f77_mpi_op_create
#define mpi_op_free_                   mpi_op_free
#define mpi_pack_                      mpi_pack
#define mpi_pack_size_                 mpi_pack_size
#define mpi_recv_                      mpi_recv
#define mpi_reduce_                    mpi_reduce
#define mpi_rsend_                     mpi_rsend
#define mpi_send_                      mpi_send
#define mpi_sendrecv_                  mpi_sendrecv
#define bi_f77_mpi_test_               bi_f77_mpi_test
#define bi_f77_mpi_testall_            bi_f77_mpi_testall
#define mpi_type_commit_               mpi_type_commit
#define mpi_type_contiguous_           mpi_type_contiguous
#define mpi_type_free_                 mpi_type_free
#define mpi_type_indexed_              mpi_type_indexed
#define mpi_type_struct_               mpi_type_struct
#define mpi_type_vector_               mpi_type_vector
#define mpi_unpack_                    mpi_unpack
#define mpi_waitall_                   mpi_waitall
#define mpi_wtime_                     mpi_wtime
#define bi_f77_get_constants_          bi_f77_get_constants
#define bi_f77_init_                   bi_f77_init
#else
#define mpi_init_                    mpi_init
#define bi_f77_get_constants_        bi_f77_get_constants
#define bi_f77_init_                 bi_f77_init
#if (BI_TransComm == BONEHEAD)
#define mpi_comm_group_              mpi_comm_group
#define mpi_group_translate_ranks_   mpi_group_translate_ranks
#endif
#endif

#endif

#if (F77_CALL_C == UPCASE)
/*
 * These defines set up the naming scheme required to have a fortran 77
 * routine call a C routine (which is what the BLACS are written in)
 * for the following Fortran to C interface:
 *           FORTRAN CALL               C DECLARATION
 *           call dgebs2d(...)          void DGEBS2D(...)
 */
/*
 * Support routines
 */
#define blacs_pinfo_                   BLACS_PINFO
#define blacs_setup_                   BLACS_SETUP
#define setpvmtids_                    SETPVMTIDS
#define blacs_set_                     BLACS_SET
#define blacs_get_                     BLACS_GET
#define blacs_gridinit_                BLACS_GRIDINIT
#define blacs_gridmap_                 BLACS_GRIDMAP
#define ksendid_                       KSENDID
#define krecvid_                       KRECVID
#define kbsid_                         KBSID
#define kbrid_                         KBRID
#define blacs_freebuff_                BLACS_FREEBUFF
#define blacs_gridexit_                BLACS_GRIDEXIT
#define blacs_abort_                   BLACS_ABORT
#define blacs_exit_                    BLACS_EXIT
#define blacs_gridinfo_                BLACS_GRIDINFO
#define blacs_pnum_                    BLACS_PNUM
#define blacs_pcoord_                  BLACS_PCOORD
#define dcputime00_                    DCPUTIME00
#define dwalltime00_                   DWALLTIME00
#define blacs_barrier_                 BLACS_BARRIER

/*
 * Main, type dependent, routines
 */
#define igesd2d_   IGESD2D
#define igerv2d_   IGERV2D
#define igebs2d_   IGEBS2D
#define igebr2d_   IGEBR2D
#define itrsd2d_   ITRSD2D
#define itrrv2d_   ITRRV2D
#define itrbs2d_   ITRBS2D
#define itrbr2d_   ITRBR2D
#define igsum2d_   IGSUM2D
#define igamx2d_   IGAMX2D
#define igamn2d_   IGAMN2D
#define sgesd2d_   SGESD2D
#define sgerv2d_   SGERV2D
#define sgebs2d_   SGEBS2D
#define sgebr2d_   SGEBR2D
#define strsd2d_   STRSD2D
#define strrv2d_   STRRV2D
#define strbs2d_   STRBS2D
#define strbr2d_   STRBR2D
#define sgsum2d_   SGSUM2D
#define sgamx2d_   SGAMX2D
#define sgamn2d_   SGAMN2D
#define dgesd2d_   DGESD2D
#define dgerv2d_   DGERV2D
#define dgebs2d_   DGEBS2D
#define dgebr2d_   DGEBR2D
#define dtrsd2d_   DTRSD2D
#define dtrrv2d_   DTRRV2D
#define dtrbs2d_   DTRBS2D
#define dtrbr2d_   DTRBR2D
#define dgsum2d_   DGSUM2D
#define dgamx2d_   DGAMX2D
#define dgamn2d_   DGAMN2D
#define cgesd2d_   CGESD2D
#define cgerv2d_   CGERV2D
#define cgebs2d_   CGEBS2D
#define cgebr2d_   CGEBR2D
#define ctrsd2d_   CTRSD2D
#define ctrrv2d_   CTRRV2D
#define ctrbs2d_   CTRBS2D
#define ctrbr2d_   CTRBR2D
#define cgsum2d_   CGSUM2D
#define cgamx2d_   CGAMX2D
#define cgamn2d_   CGAMN2D
#define zgesd2d_   ZGESD2D
#define zgerv2d_   ZGERV2D
#define zgebs2d_   ZGEBS2D
#define zgebr2d_   ZGEBR2D
#define ztrsd2d_   ZTRSD2D
#define ztrrv2d_   ZTRRV2D
#define ztrbs2d_   ZTRBS2D
#define ztrbr2d_   ZTRBR2D
#define zgsum2d_   ZGSUM2D
#define zgamx2d_   ZGAMX2D
#define zgamn2d_   ZGAMN2D

/*
 * If we are using the fortran interface to MPI, need to redefine the names
 */
#ifdef UseF77Mpi
#define mpi_abort_                     MPI_ABORT
#define mpi_allreduce_                 MPI_ALLREDUCE
#define bi_f77_mpi_attr_get_           BI_F77_MPI_ATTR_GET
#define mpi_barrier_                   MPI_BARRIER
#define mpi_bcast_                     MPI_BCAST
#define mpi_comm_create_               MPI_COMM_CREATE
#define mpi_comm_dup_                  MPI_COMM_DUP
#define mpi_comm_free_                 MPI_COMM_FREE
#define mpi_comm_group_                MPI_COMM_GROUP
#define mpi_comm_rank_                 MPI_COMM_RANK
#define mpi_comm_size_                 MPI_COMM_SIZE
#define mpi_comm_split_                MPI_COMM_SPLIT
#define mpi_error_class_               MPI_ERROR_CLASS
#define mpi_finalize_                  MPI_FINALIZE
#define mpi_get_count_                 MPI_GET_COUNT
#define mpi_group_incl_                MPI_GROUP_INCL
#define mpi_group_free_                MPI_GROUP_FREE
#define mpi_init_                      MPI_INIT
#define bi_f77_mpi_initialized_        BI_F77_MPI_INITIALIZED
#define mpi_irecv_                     MPI_IRECV
#define mpi_isend_                     MPI_ISEND
#define bi_f77_mpi_op_create_          BI_F77_MPI_OP_CREATE
#define mpi_op_free_                   MPI_OP_FREE
#define mpi_pack_                      MPI_PACK
#define mpi_pack_size_                 MPI_PACK_SIZE
#define mpi_recv_                      MPI_RECV
#define mpi_reduce_                    MPI_REDUCE
#define mpi_rsend_                     MPI_RSEND
#define mpi_send_                      MPI_SEND
#define mpi_sendrecv_                  MPI_SENDRECV
#define bi_f77_mpi_test_               BI_F77_MPI_TEST
#define bi_f77_mpi_testall_            BI_F77_MPI_TESTALL
#define mpi_type_commit_               MPI_TYPE_COMMIT
#define mpi_type_contiguous_           MPI_TYPE_CONTIGUOUS
#define mpi_type_free_                 MPI_TYPE_FREE
#define mpi_type_indexed_              MPI_TYPE_INDEXED
#define mpi_type_struct_               MPI_TYPE_STRUCT
#define mpi_type_vector_               MPI_TYPE_VECTOR
#define mpi_unpack_                    MPI_UNPACK
#define mpi_waitall_                   MPI_WAITALL
#define mpi_wtime_                     MPI_WTIME
#define bi_f77_get_constants_          BI_F77_GET_CONSTANTS
#define bi_f77_init_                   BI_F77_INIT
#else
#define mpi_init_                    MPI_INIT
#define bi_f77_init_                 BI_F77_INIT
#define bi_f77_get_constants_        BI_F77_GET_CONSTANTS
#if (BI_TransComm == BONEHEAD)
#define mpi_comm_group_             MPI_COMM_GROUP
#define mpi_group_translate_ranks_  MPI_GROUP_TRANSLATE_RANKS
#endif
#endif

#endif

#if (F77_CALL_C == F77ISF2C)
/*
 * These defines set up the naming scheme required to have a fortran 77
 * routine call a C routine (which is what the BLACS are written in)
 * for systems where the fortran "compiler" is actually f2c (a fortran
 * to C conversion utility).
 */
/*
 * Initialization routines
 */
#define blacs_pinfo_    blacs_pinfo__
#define blacs_setup_    blacs_setup__
#define blacs_set_      blacs_set__
#define blacs_get_      blacs_get__
#define blacs_gridinit_ blacs_gridinit__
#define blacs_gridmap_  blacs_gridmap__
/*
 * Destruction routines
 */
#define blacs_freebuff_ blacs_freebuff__
#define blacs_gridexit_ blacs_gridexit__
#define blacs_abort_    blacs_abort__
#define blacs_exit_     blacs_exit__
/*
 * Informational & misc.
 */
#define blacs_gridinfo_ blacs_gridinfo__
#define blacs_pnum_     blacs_pnum__
#define blacs_pcoord_   blacs_pcoord__
#define blacs_barrier_  blacs_barrier__

/*
 * If we are using the fortran interface to MPI, need to redefine the names
 */
#ifdef UseF77Mpi
#define mpi_abort_                     mpi_abort__
#define mpi_allreduce_                 mpi_allreduce__
#define mpi_barrier_                   mpi_barrier__
#define mpi_bcast_                     mpi_bcast__
#define mpi_init_                      mpi_init__
#define mpi_finalize_                  mpi_finalize__
#define mpi_irecv_                     mpi_irecv__
#define mpi_pack_                      mpi_pack__
#define mpi_isend_                     mpi_isend__
#define mpi_recv_                      mpi_recv__
#define mpi_reduce_                    mpi_reduce__
#define mpi_rsend_                     mpi_rsend__
#define mpi_send_                      mpi_send__
#define mpi_sendrecv_                  mpi_sendrecv__
#define mpi_unpack_                    mpi_unpack__
#define mpi_waitall_                   mpi_waitall__
#define mpi_wtime_                     mpi_wtime__

#define mpi_comm_create_               mpi_comm_create__
#define mpi_comm_dup_                  mpi_comm_dup__
#define mpi_comm_free_                 mpi_comm_free__
#define mpi_comm_group_                mpi_comm_group__
#define mpi_comm_rank_                 mpi_comm_rank__
#define mpi_comm_size_                 mpi_comm_size__
#define mpi_comm_split_                mpi_comm_split__
#define mpi_error_class_               mpi_error_class__
#define mpi_get_count_                 mpi_get_count__
#define mpi_group_incl_                mpi_group_incl__
#define mpi_group_free_                mpi_group_free__
#define mpi_op_free_                   mpi_op_free__
#define mpi_pack_size_                 mpi_pack_size__
#define mpi_type_commit_               mpi_type_commit__
#define mpi_type_contiguous_           mpi_type_contiguous__
#define mpi_type_free_                 mpi_type_free__
#define mpi_type_indexed_              mpi_type_indexed__
#define mpi_type_struct_               mpi_type_struct__
#define mpi_type_vector_               mpi_type_vector__

#define bi_f77_get_constants_          bi_f77_get_constants__
#define bi_f77_init_                   bi_f77_init__
#define mpi_group_translate_ranks_     mpi_group_translate_ranks__
#define bi_f77_mpi_initialized_        bi_f77_mpi_initialized__
#define bi_f77_mpi_test_               bi_f77_mpi_test__
#define bi_f77_mpi_testall_            bi_f77_mpi_testall__

#define bi_f77_mpi_attr_get_           bi_f77_mpi_attr_get__
#define bi_f77_mpi_op_create_          bi_f77_mpi_op_create__
#else
#define mpi_init_                   mpi_init__
#define bi_f77_get_constants_       bi_f77_get_constants__
#define bi_f77_init_                bi_f77_init__
#if (BI_TransComm == BONEHEAD)
#define mpi_comm_group_             mpi_comm_group__
#define mpi_group_translate_ranks_  mpi_group_translate_ranks__
#endif
#endif

#endif

/*
 * ==========================================================================
 * Prototype the fortran interface MPI functions if they are going to be used
 * ==========================================================================
 */
#ifdef UseF77Mpi

#ifdef __STDC__
   F_VOID_FUNC mpi_abort_(int*, int*, int*);
   F_VOID_FUNC mpi_allreduce_(void*, void*, int*, int*, int*, int*, int*);
   F_VOID_FUNC bi_f77_mpi_attr_get_(int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_barrier_(int*, int*);
   F_VOID_FUNC mpi_bcast_(void*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_comm_create_(int*, int*, int*, int*);
   F_VOID_FUNC mpi_comm_dup_(int*, int*, int*);
   F_VOID_FUNC mpi_comm_free_(int*, int*);
   F_VOID_FUNC mpi_comm_group_(int*, int*, int*);
   F_VOID_FUNC mpi_comm_rank_(int*, int*, int*);
   F_VOID_FUNC mpi_comm_size_(int*, int*, int*);
   F_VOID_FUNC mpi_comm_split_(int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_error_class_(int*, int*, int*);
   F_VOID_FUNC mpi_finalize_(int*);
   F_VOID_FUNC mpi_get_count_(int*, int*, int*, int*);
   F_VOID_FUNC mpi_group_incl_(int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_group_free_(int*, int*);
   F_VOID_FUNC mpi_init_(int*);
   F_VOID_FUNC bi_f77_mpi_initialized_(int*, int*);
   F_VOID_FUNC mpi_irecv_(void*, int*, int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_isend_(void*, int*, int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC bi_f77_mpi_op_create_(void func(void*, void*, int*, int*),
                                     int*, int*, int*);
   F_VOID_FUNC mpi_op_free_(int*, int*);
   F_VOID_FUNC mpi_pack_(void*, int*, int*, void*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_pack_size_(int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_recv_(void *, int*, int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_reduce_(void*, void*, int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_rsend_(void*, int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_send_(void*, int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_sendrecv_(void*, int*, int*, int*, int*, void*, int*, int*,
                             int*, int*, int*, int*, int*);
   F_VOID_FUNC bi_f77_mpi_test_(int*, int*, int*, int*);
   F_VOID_FUNC bi_f77_mpi_testall_(int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_type_commit_(int*, int*);
   F_VOID_FUNC mpi_type_contiguous_(int*, int*, int*, int*);
   F_VOID_FUNC mpi_type_free_(int*, int*);
   F_VOID_FUNC mpi_type_indexed_(int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_type_struct_(int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_type_vector_(int*, int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_unpack_(void*, int*, int*, void*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_waitall_(int*, int*, int*, int*);
   F_DOUBLE_FUNC mpi_wtime(void);
#else
   F_VOID_FUNC mpi_abort_();
   F_VOID_FUNC mpi_allreduce_();
   F_VOID_FUNC bi_f77_mpi_attr_get_();
   F_VOID_FUNC mpi_barrier_();
   F_VOID_FUNC mpi_bcast_();
   F_VOID_FUNC mpi_comm_create_();
   F_VOID_FUNC mpi_comm_dup_();
   F_VOID_FUNC mpi_comm_free_();
   F_VOID_FUNC mpi_comm_group_();
   F_VOID_FUNC mpi_comm_rank_();
   F_VOID_FUNC mpi_comm_size_();
   F_VOID_FUNC mpi_comm_split_();
   F_VOID_FUNC mpi_error_class_();
   F_VOID_FUNC mpi_finalize_();
   F_VOID_FUNC mpi_get_count_();
   F_VOID_FUNC mpi_group_incl_();
   F_VOID_FUNC mpi_group_free_();
   F_VOID_FUNC mpi_init_();
   F_VOID_FUNC bi_f77_mpi_initialized_();
   F_VOID_FUNC mpi_irecv_();
   F_VOID_FUNC mpi_isend_();
   F_VOID_FUNC bi_f77_mpi_op_create_();
   F_VOID_FUNC mpi_op_free_();
   F_VOID_FUNC mpi_pack_();
   F_VOID_FUNC mpi_pack_size_();
   F_VOID_FUNC mpi_recv_();
   F_VOID_FUNC mpi_reduce_();
   F_VOID_FUNC mpi_rsend_();
   F_VOID_FUNC mpi_send_();
   F_VOID_FUNC mpi_sendrecv_();
   F_VOID_FUNC bi_f77_mpi_test_();
   F_VOID_FUNC bi_f77_mpi_testall_();
   F_VOID_FUNC mpi_type_commit_();
   F_VOID_FUNC mpi_type_contiguous_();
   F_VOID_FUNC mpi_type_free_();
   F_VOID_FUNC mpi_type_indexed_();
   F_VOID_FUNC mpi_type_struct_();
   F_VOID_FUNC mpi_type_vector_();
   F_VOID_FUNC mpi_unpack_();
   F_VOID_FUNC mpi_waitall_();
   F_DOUBLE_FUNC BI_MPI_Wtime();
#endif

/*
 * If we are using the C interface, still may need some f77 functions to do
 * context translation
 */
#else

#if (BI_TransComm == BONEHEAD)

#ifdef __STDC__
   F_VOID_FUNC mpi_comm_create_(int*, int*, int*, int*);
   F_VOID_FUNC mpi_comm_free_(int*, int*);
   F_VOID_FUNC mpi_comm_group_(int*, int*, int*);
   F_VOID_FUNC mpi_comm_size_(int*, int*, int*);
   F_VOID_FUNC mpi_group_incl_(int*, int*, int*, int*, int*);
   F_VOID_FUNC mpi_group_free_(int*, int*);
   F_VOID_FUNC mpi_group_translate_ranks_(int*, int*, int*, int*, int*, int*);
#else
   F_VOID_FUNC mpi_comm_create_();
   F_VOID_FUNC mpi_comm_free_();
   F_VOID_FUNC mpi_comm_group_();
   F_VOID_FUNC mpi_comm_size_();
   F_VOID_FUNC mpi_group_incl_();
   F_VOID_FUNC mpi_group_free_();
   F_VOID_FUNC mpi_group_translate_ranks_();
#endif

#endif

#endif

/*
 * ================================================
 * Define MPI functions for C and fortran interface
 * ================================================
 */

#ifdef UseF77Mpi
#define BI_MPI_Abort(comm_, errcode_, ierr_)\
{ \
   mpi_abort_(&(comm_), &(errcode_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_ABORT", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Abort(comm_, errcode_, ierr_)\
{ \
   (ierr_) = MPI_Abort((comm_), (errcode_));  \
   Mmpierror((ierr_), "MPI_Abort", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Allreduce(sbuf_, rbuf_, count_, dtype_, op_, comm_, ierr_)\
{ \
   mpi_allreduce_((sbuf_), (rbuf_), &(count_), &(dtype_), &(op_), &(comm_), \
                  &(ierr_)); \
   Mmpierror((ierr_), "MPI_ALLREDUCE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Allreduce(sbuf_, rbuf_, count_, dtype_, op_, comm_, ierr_)\
{ \
   (ierr_) = MPI_Allreduce((sbuf_), (rbuf_), (count_), (dtype_), (op_), \
                           (comm_)); \
   Mmpierror((ierr_), "MPI_Allreduce", NULL, __LINE__, __FILE__); \
}
#endif

/*
 * Need to be sure to set attr_ to pointing at int before call . . .
 * int *iptr=&i; BI_MPI_Attr_get(... &iptr ...);
 */
#ifdef UseF77Mpi
#define BI_MPI_Attr_get(comm_, keyval_, attr_, flag_, ierr_) \
{ \
   bi_f77_mpi_attr_get_(&(comm_), &(keyval_), (int *) *(attr_), (flag_), \
                        &(ierr_)); \
   Mmpierror((ierr_), "MPI_ATTR_GET", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Attr_get(comm_, keyval_, attr_, flag_, ierr_) \
{ \
   (ierr_) = MPI_Attr_get((comm_), (keyval_), (attr_), (flag_)); \
   Mmpierror((ierr_), "MPI_Attr_get", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Barrier(comm_, ierr_) \
{ \
   mpi_barrier_(&(comm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_BARRIER", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Barrier(comm_, ierr_) \
{ \
   (ierr_) = MPI_Barrier((comm_)); \
   Mmpierror((ierr_), "MPI_Barrier", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Bcast(buff_, count_, dtype_, root_, comm_, ierr_) \
{ \
   mpi_bcast_((buff_), &(count_), &(dtype_), &(root_), &(comm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_BCAST", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Bcast(buff_, count_, dtype_, root_, comm_, ierr_) \
{ \
   (ierr_) = MPI_Bcast((buff_), (count_), (dtype_), (root_), (comm_)); \
   Mmpierror((ierr_), "MPI_Bcast", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Comm_create(comm_, group_, newcomm_, ierr_) \
{ \
   mpi_comm_create_(&(comm_), &(group_), (newcomm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_COMM_CREATE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Comm_create(comm_, group_, newcomm_, ierr_) \
{ \
   (ierr_) = MPI_Comm_create((comm_), (group_), (newcomm_)); \
   Mmpierror((ierr_), "MPI_Comm_create", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Comm_dup(comm_, newcomm_, ierr_) \
{ \
   mpi_comm_dup_(&(comm_), (newcomm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_COMM_DUP", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Comm_dup(comm_, newcomm_, ierr_) \
{ \
   (ierr_) = MPI_Comm_dup((comm_), (newcomm_)); \
   Mmpierror((ierr_), "MPI_Comm_dup", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Comm_free(comm_, ierr_) \
{ \
   mpi_comm_free_((comm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_COMM_FREE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Comm_free(comm_, ierr_) \
{ \
   (ierr_) = MPI_Comm_free((comm_)); \
   Mmpierror((ierr_), "MPI_Comm_free", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Comm_group(comm_, grp_, ierr_) \
{ \
   mpi_comm_group_(&(comm_), (grp_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_COMM_GROUP", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Comm_group(comm_, grp_, ierr_) \
{ \
   (ierr_) = MPI_Comm_group((comm_), (grp_)); \
   Mmpierror((ierr_), "MPI_Comm_group", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Comm_rank(comm_, rank_, ierr_) \
{ \
   mpi_comm_rank_(&(comm_), (rank_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_COMM_RANK", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Comm_rank(comm_, rank_, ierr_) \
{ \
   (ierr_) = MPI_Comm_rank((comm_), (rank_)); \
   Mmpierror((ierr_), "MPI_Comm_rank", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Comm_size(comm_, size_, ierr_) \
{ \
   mpi_comm_size_(&(comm_), (size_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_COMM_SIZE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Comm_size(comm_, size_, ierr_) \
{ \
   (ierr_) = MPI_Comm_size((comm_), (size_)); \
   Mmpierror((ierr_), "MPI_Comm_size", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Comm_split(comm_, color_, key_, newcomm_, ierr_) \
{ \
   mpi_comm_split_(&(comm_), &(color_), &(key_), (newcomm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_COMM_SPLIT", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Comm_split(comm_, color_, key_, newcomm_, ierr_) \
{ \
   (ierr_) = MPI_Comm_split((comm_), (color_), (key_), (newcomm_)); \
   Mmpierror((ierr_), "MPI_Comm_split", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Error_class(errcode_, errclass_, ierr_) \
{ \
   mpi_error_class_(&(errcode_), (errclass_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_ERROR_CLASS", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Error_class(errcode_, errclass_, ierr_) \
{ \
   (ierr_) = MPI_Error_class((errcode_), (errclass_)); \
   Mmpierror((ierr_), "MPI_Error_class", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Finalize(ierr_) \
{ \
   mpi_finalize_(&(ierr_)); \
   Mmpierror((ierr_), "MPI_FINALIZE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Finalize(ierr_) \
{ \
   (ierr_) = MPI_Finalize(); \
   Mmpierror((ierr_), "MPI_Finalize", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Get_count(stat_, dtype_, count_, ierr_) \
{ \
   mpi_get_count_((stat_), &(dtype_), (count_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_GET_COUNT", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Get_count(stat_, dtype_, count_, ierr_) \
{ \
   (ierr_) = MPI_Get_count((stat_), (dtype_), (count_)); \
   Mmpierror((ierr_), "MPI_Get_count", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Group_free(grp_, ierr_) \
{ \
   mpi_group_free_((grp_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_GROUP_FREE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Group_free(grp_, ierr_) \
{ \
   (ierr_) = MPI_Group_free(grp_); \
   Mmpierror((ierr_), "MPI_Group_free", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Group_incl(grp_, n_, ranks_, newgrp_, ierr_) \
{ \
   mpi_group_incl_(&(grp_), &(n_), (ranks_), (newgrp_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_GROUP_INCL", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Group_incl(grp_, n_, ranks_, newgrp_, ierr_) \
{ \
   (ierr_) = MPI_Group_incl((grp_), (n_), (ranks_), (newgrp_)); \
   Mmpierror((ierr_), "MPI_Group_incl", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Group_translate_ranks(grp1_, n_, ranks1_, grp2_, ranks2_, ierr_) \
{ \
   MPI_Group_translate_ranks(&(grp1_), &(n_), (ranks1_), &(grp2_), (ranks2_),\
                             &(ierr_)); \
   Mmpierror((ierr_), "MPI_GROUP_TRANSLATE_RANKS", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Group_translate_ranks(grp1_, n_, ranks1_, grp2_, ranks2_, ierr_) \
{ \
   (ierr_) = MPI_Group_translate_ranks((grp1_), (n_), (ranks1_), (grp2_),\
                                       (ranks2_)); \
   Mmpierror((ierr_), "MPI_Group_translate_ranks", NULL, __LINE__, __FILE__); \
}
#endif

/*
 *  The BLACS always call f77's mpi_init.  If the user is using C, he should
 *  explicitly call MPI_Init . . .
 */
#define BI_MPI_Init(argc_, argv_, ierr_) \
{ \
   mpi_init_(&(ierr_)); \
   Mmpierror((ierr_), "MPI_INIT", NULL, __LINE__, __FILE__); \
}

#ifdef UseF77Mpi
#define BI_MPI_Initialized(flag_, ierr_) \
{ \
   bi_f77_mpi_initialized_((flag_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_INITIALIZED", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Initialized(flag_, ierr_) \
{ \
   (ierr_) = MPI_Initialized((flag_)); \
   Mmpierror((ierr_), "MPI_Initialized", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Irecv(buf_, count_, dtype_, src_, tag_, comm_, req_, ierr_) \
   mpi_irecv_((buf_), &(count_), &(dtype_), &(src_), &(tag_), &(comm_), \
              (req_), &(ierr_))
#else
#define BI_MPI_Irecv(buf_, count_, dtype_, src_, tag_, comm_, req_, ierr_) \
   (ierr_) = MPI_Irecv((buf_), (count_), (dtype_), (src_), (tag_), (comm_), \
                       (req_))
#endif

#ifdef UseF77Mpi
#define BI_MPI_Isend(buf_, count_, dtype_, dest_, tag_, comm_, req_, ierr_) \
   mpi_isend_((buf_), &(count_), &(dtype_), &(dest_), &(tag_), &(comm_), \
              (req_), &(ierr_));
#else
#define BI_MPI_Isend(buf_, count_, dtype_, dest_, tag_, comm_, req_, ierr_) \
   (ierr_) = MPI_Isend((buf_), (count_), (dtype_), (dest_), (tag_), (comm_), \
                       (req_))
#endif

#ifdef UseF77Mpi
#define BI_MPI_Op_create(func_, commute_, op_, ierr_) \
{ \
   bi_f77_mpi_op_create_((func_), &(commute_), (op_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_OP_CREATE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Op_create(func_, commute_, op_, ierr_) \
{ \
   (ierr_) = MPI_Op_create((func_), (commute_), (op_)); \
   Mmpierror((ierr_), "MPI_Op_create", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Op_free(op_, ierr_) \
{ \
   mpi_op_free_((op_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_OP_FREE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Op_free(op_, ierr_) \
{ \
   (ierr_) = MPI_Op_free((op_)); \
   Mmpierror((ierr_), "MPI_Op_free", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Pack(inbuf_, incount_, dtype_, outbuf_, outsiz_, pos_, comm_, ierr_) \
{ \
   mpi_pack_((inbuf_), &(incount_), &(dtype_), (outbuf_), &(outsiz_), \
             (pos_), &(comm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_PACK", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Pack(inbuf_, incount_, dtype_, outbuf_, outsiz_, pos_, comm_, ierr_) \
{ \
   (ierr_) = MPI_Pack((inbuf_), (incount_), (dtype_), (outbuf_), (outsiz_), \
                      (pos_), (comm_)); \
   Mmpierror((ierr_), "MPI_Pack", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Pack_size(incount_, dtype_, comm_, size_, ierr_) \
{ \
   mpi_pack_size_(&(incount_), &(dtype_), &(comm_), (size_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_PACK_SIZE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Pack_size(incount_, dtype_, comm_, size_, ierr_) \
{ \
   (ierr_) = MPI_Pack_size((incount_), (dtype_), (comm_), (size_)); \
   Mmpierror((ierr_), "MPI_Pack_size", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Recv(buf_, count_, dtype_, src_, tag_, comm_, stat_, ierr_) \
{ \
   mpi_recv_((buf_), &(count_), &(dtype_), &(src_), &(tag_), &(comm_), \
             (stat_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_RECV", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Recv(buf_, count_, dtype_, src_, tag_, comm_, stat_, ierr_) \
{ \
   (ierr_) = MPI_Recv((buf_), (count_), (dtype_), (src_), (tag_), (comm_), \
                      (stat_)); \
   Mmpierror((ierr_), "MPI_Recv", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Reduce(sbuf_, rbuf_, count_, dtype_, op_, root_, comm_, ierr_) \
{ \
   mpi_reduce_((sbuf_), (rbuf_), &(count_), &(dtype_), &(op_), &(root_), \
               &(comm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_REDUCE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Reduce(sbuf_, rbuf_, count_, dtype_, op_, root_, comm_, ierr_) \
{ \
   (ierr_) = MPI_Reduce((sbuf_), (rbuf_), (count_), (dtype_), (op_), (root_), \
                        (comm_)); \
   Mmpierror((ierr_), "MPI_Reduce", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Rsend(buf_, count_, dtype_, dest_, tag_, comm_, ierr_) \
{ \
   mpi_rsend_((buf_), &(count_), &(dtype_), &(dest_), &(tag_), &(comm_), \
              &(ierr_)); \
   Mmpierror((ierr_), "MPI_RSEND", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Rsend(buf_, count_, dtype_, dest_, tag_, comm_, ierr_) \
{ \
   (ierr_) = MPI_Rsend((buf_), (count_), (dtype_), (dest_), (tag_), (comm_)); \
   Mmpierror((ierr_), "MPI_Rsend", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Send(buf_, count_, dtype_, dest_, tag_, comm_, ierr_) \
{ \
   mpi_send_((buf_), &(count_), &(dtype_), &(dest_), &(tag_), &(comm_), \
             &(ierr_)); \
   Mmpierror((ierr_), "MPI_SEND", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Send(buf_, count_, dtype_, dest_, tag_, comm_, ierr_) \
{ \
   (ierr_) = MPI_Send((buf_), (count_), (dtype_), (dest_), (tag_), (comm_)); \
   Mmpierror((ierr_), "MPI_Send", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Sendrecv(sbuf_, scount_, stype_, dest_, stag_, rbuf_, rcount_, rtype_, src_, rtag_, comm_, stat_, ierr_) \
{ \
   mpi_sendrecv_((sbuf_), &(scount_), &(stype_), &(dest_), &(stag_), \
                 (rbuf_), &(rcount_), &(rtype_), &(src_), &(rtag_), \
                 &(comm_), (stat_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_SENDRECV", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Sendrecv(sbuf_, scount_, stype_, dest_, stag_, rbuf_, rcount_, rtype_, src_, rtag_, comm_, stat_, ierr_) \
{ \
   (ierr_) = MPI_Sendrecv((sbuf_), (scount_), (stype_), (dest_), (stag_), \
                          (rbuf_), (rcount_), (rtype_), (src_), (rtag_), \
                          (comm_), (stat_)); \
   Mmpierror((ierr_), "MPI_Sendrecv", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Test(req_, flag_, stat_, ierr_) \
{ \
   bi_f77_mpi_test_(&(req_), (flag_), (stat_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_TEST", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Test(req_, flag_, stat_, ierr_) \
{ \
   (ierr_) = MPI_Test((req_), (flag_), (stat_)); \
   Mmpierror((ierr_), "MPI_Test", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Testall(count_, reqs_, flag_, stats_, ierr_) \
{ \
   bi_f77_mpi_testall_(&(count_), (reqs_), (flag_), (stats_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_TESTALL", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Testall(count_, reqs_, flag_, stats_, ierr_) \
{ \
   (ierr_) = MPI_Testall((count_), (reqs_), (flag_), (stats_)); \
   Mmpierror((ierr_), "MPI_Testall", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Type_commit(dtype_, ierr_) \
{ \
   mpi_type_commit_((dtype_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_TYPE_COMMIT", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Type_commit(dtype_, ierr_) \
{ \
   (ierr_) = MPI_Type_commit((dtype_)); \
   Mmpierror((ierr_), "MPI_Type_commit", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Type_contiguous(count_, oldtype_, newtype_, ierr_) \
{ \
   mpi_type_contiguous_(&(count_), &(oldtype_), (newtype_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_TYPE_CONTIGUOUS", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Type_contiguous(count_, oldtype_, newtype_, ierr_) \
{ \
   (ierr_) = MPI_Type_contiguous((count_), (oldtype_), (newtype_)); \
   Mmpierror((ierr_), "MPI_Type_contiguous", NULL, __LINE__, __FILE__); \
}
#endif

/*
 * Some versions of mpich and its derivitives cannot handle 0 byte typedefs,
 * so we set type MPI_BYTE as a flag for a 0 byte message
 */
#ifdef ZeroByteTypeBug

#ifdef UseF77Mpi
#define BI_MPI_Type_free(dtype_, ierr_) \
{ \
   if ( *(dtype_) != BI_MPI_BYTE) \
   { \
      mpi_type_free_((dtype_), &(ierr_)); \
      Mmpierror((ierr_), "MPI_TYPE_FREE", NULL, __LINE__, __FILE__); \
   } \
}
#else
#define BI_MPI_Type_free(dtype_, ierr_) \
{ \
   if ( *(dtype_) != BI_MPI_BYTE) \
   { \
      (ierr_) = MPI_Type_free((dtype_)); \
      Mmpierror((ierr_), "MPI_Type_free", NULL, __LINE__, __FILE__); \
   } \
}
#endif

#else

#ifdef UseF77Mpi
#define BI_MPI_Type_free(dtype_, ierr_) \
{ \
   mpi_type_free_((dtype_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_TYPE_FREE", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Type_free(dtype_, ierr_) \
{ \
   (ierr_) = MPI_Type_free((dtype_)); \
   Mmpierror((ierr_), "MPI_Type_free", NULL, __LINE__, __FILE__); \
}
#endif

#endif

#ifdef UseF77Mpi
#define BI_MPI_Type_indexed(count_, lens_, disp_, oldtype_, newtype_, ierr_) \
{ \
   mpi_type_indexed_(&(count_), (lens_), (disp_), &(oldtype_), (newtype_), \
                     &(ierr_)); \
   Mmpierror((ierr_), "MPI_TYPE_INDEXED", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Type_indexed(count_, lens_, disp_, oldtype_, newtype_, ierr_) \
{ \
   (ierr_) = MPI_Type_indexed((count_), (lens_), (disp_), (oldtype_), \
                              (newtype_)); \
   Mmpierror((ierr_), "MPI_Type_indexed", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Type_struct(count_, lens_, disps_, types_, newtype_, ierr_) \
{ \
   mpi_type_struct_(&(count_), (lens_), (disps_), (types_), (newtype_), \
                   &(ierr_)); \
   Mmpierror((ierr_), "MPI_TYPE_STRUCT", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Type_struct(count_, lens_, disps_, types_, newtype_, ierr_) \
{ \
   (ierr_) = MPI_Type_struct((count_), (lens_), (disps_), (types_), \
                             (newtype_)); \
   Mmpierror((ierr_), "MPI_Type_struct", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Type_vector(count_, len_, stride_, oldtype_, newtype_, ierr_) \
{ \
   mpi_type_vector_(&(count_), &(len_), &(stride_), &(oldtype_), (newtype_), \
                    &(ierr_)); \
   Mmpierror((ierr_), "MPI_TYPE_VECTOR", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Type_vector(count_, len_, stride_, oldtype_, newtype_, ierr_) \
{ \
   (ierr_) = MPI_Type_vector((count_), (len_), (stride_), (oldtype_), \
                             (newtype_)); \
   Mmpierror((ierr_), "MPI_Type_vector", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Unpack(inbuf_, insize_, pos_, outbuf_, outcount_, dtype_, comm_, ierr_) \
{ \
   mpi_unpack_((inbuf_), &(insize_), (pos_), (outbuf_), &(outcount_), \
               &(dtype_), &(comm_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_UNPACK", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Unpack(inbuf_, insize_, pos_, outbuf_, outcount_, dtype_, comm_, ierr_) \
{ \
   (ierr_) = MPI_Unpack((inbuf_), (insize_), (pos_), (outbuf_), (outcount_), \
                        (dtype_), (comm_)); \
   Mmpierror((ierr_), "MPI_Unpack", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Waitall(count_, reqs_, stats_, ierr_) \
{ \
   mpi_waitall_(&(count_), (reqs_), (stats_), &(ierr_)); \
   Mmpierror((ierr_), "MPI_WAITALL", NULL, __LINE__, __FILE__); \
}
#else
#define BI_MPI_Waitall(count_, reqs_, stats_, ierr_) \
{ \
   (ierr_) = MPI_Waitall((count_), (reqs_), (stats_)); \
   Mmpierror((ierr_), "MPI_Waitall", NULL, __LINE__, __FILE__); \
}
#endif

#ifdef UseF77Mpi
#define BI_MPI_Wtime mpi_wtime_
#else
#define BI_MPI_Wtime MPI_Wtime
#endif

#ifdef CRAY
#ifndef UseF77Mpi
#undef  BI_MPI_FLOAT
#define BI_MPI_FLOAT BI_MPI_DOUBLE
#endif
#endif

#endif
