      SUBROUTINE BTSETUP( MEM, MEMLEN, CMEM, CMEMLEN, OUTNUM,
     $                    TESTSDRV, TESTBSBR, TESTCOMB, TESTAUX,
     $                    IAM, NNODES )
*
*  -- BLACS tester (version 1.0) --
*  University of Tennessee
*  December 15, 1994
*
*     .. Scalar Arguments ..
      LOGICAL TESTSDRV, TESTBSBR, TESTCOMB, TESTAUX
      INTEGER MEMLEN, CMEMLEN, OUTNUM, IAM, NNODES
*     ..
*     .. Array Arguments ..
      INTEGER MEM(MEMLEN)
      CHARACTER*1 CMEM(CMEMLEN)
*     ..
*
*  Purpose
*  =======
*  BTSETUP:  Fills in process number array, and sets up machine on
*            dynamic systems.
*
*  Arguments
*  =========
*  MEM      (input) INTEGER array, dimension MEMSIZE
*           Scratch pad memory area.
*
*  MEMLEN   (input) INTEGER
*           Number of safe elements in MEM.
*
*  CMEM     (input) CHARACTER array, dimension CMEMSIZE
*           Scratch pad memory area.
*
*  CMEMLEN  (input) INTEGER
*           Number of safe elements in MEM.
*
*  OUTNUM   (input/output) INTEGER
*           Unit number of output file for top level error information.
*           Input for process 0.  Set to zero as output for all other
*           processes as a safety precaution.
*
*  TESTSDRV (input) LOGICAL
*           Will there be point-to-point tests in this test run?
*
*  TESTBSBR (input) LOGICAL
*           Will there be broadcast tests in this test run?
*
*  TESTCOMB (input) LOGICAL
*           Will there be combine-operator tests in this test run?
*
*  TESTAUX  (input) LOGICAL
*           Will there be auxiliary tests in this test run?
*
*  IAM      (input/output) INTEGER
*           This process's node number.
*
*  NNODES   (input/output) INTEGER
*           Number of processes that are started up by this subroutine.
*
*  ====================================================================
*
*     .. Local Scalars ..
      INTEGER I, CONTEXT, MEMUSED, CMEMUSED, NGRID, PPTR, QPTR
*     ..
*     .. External Functions ..
      INTEGER BLACS_PNUM
      EXTERNAL BLACS_PNUM
*     ..
*     .. External Subroutines ..
      EXTERNAL BLACS_SETUP, BLACS_GRIDINIT, BLACS_GRIDEXIT
*     ..
*     .. Common blocks ..
      COMMON /BTPNUM/ BTPNUMS
*     ..
*     .. Arrays in Common ..
      INTEGER BTPNUMS(0:999)
*     ..
*     .. Executable Statements ..
*
      IF( NNODES .GT. 0 ) RETURN
      IF ( IAM .EQ. 0 ) THEN
         IF ( TESTSDRV ) THEN
*
*           Determine the max number of nodes required by a SDRV tests
*
            CALL RDSDRV( MEMUSED, MEM, MEMLEN, CMEMUSED, CMEM, CMEMLEN,
     $                   OUTNUM )
            IF( (MEMUSED + 24) .GT. MEMLEN ) THEN
               WRITE(OUTNUM, *) 'Not enough memory to read in sdrv.dat'
               STOP
            END IF
*
            I = MEMUSED + 1
            CALL BTUNPACK( 'SDRV', MEM, MEMUSED, MEM(I+1), MEM(I+2),
     $                     MEM(I+22), MEM(I+23), MEM(I+12), MEM(I+20),
     $                     MEM(I+3), MEM(I+13), NGRID, MEM(I+4),
     $                     MEM(I+14), MEM(I+21), MEM(I+5), MEM(I+15),
     $                     MEM(I+6), MEM(I+16), MEM(I+7), MEM(I+17),
     $                     MEM(I+8), MEM(I+18), MEM(I+9), MEM(I+19),
     $                     MEM(I+11), PPTR, QPTR )
*
            DO 10 I = 0, NGRID-1
               NNODES = MAX0( MEM(PPTR+I) * MEM(QPTR+I), NNODES )
   10       CONTINUE
         END IF
         IF( TESTBSBR ) THEN
*
*           Determine the maximum number of nodes required by a
*           broadcast test case
*
            CALL RDBSBR( MEMUSED, MEM, MEMLEN, CMEMUSED, CMEM, CMEMLEN,
     $                   OUTNUM )
            I = MEMUSED + 1
            CALL BTUNPACK( 'BSBR', MEM, MEMUSED, MEM(I+1), MEM(I+2),
     $                     MEM(I+22), MEM(I+23), MEM(I+12), MEM(I+20),
     $                     MEM(I+3), MEM(I+13), NGRID, MEM(I+4),
     $                     MEM(I+14), MEM(I+21), MEM(I+5), MEM(I+15),
     $                     MEM(I+6), MEM(I+16), MEM(I+7), MEM(I+17),
     $                     MEM(I+8), MEM(I+18), MEM(I+9), MEM(I+19),
     $                     MEM(I+11), PPTR, QPTR )
            DO 20 I = 0, NGRID-1
               NNODES = MAX0( MEM(PPTR+I) * MEM(QPTR+I), NNODES )
   20       CONTINUE
*
         END IF
         IF( TESTCOMB ) THEN
*
*           Determine the maximum number of nodes required by a
*           combine test case
*
            CALL RDCOMB( MEMUSED, MEM, MEMLEN, CMEMUSED, CMEM, CMEMLEN,
     $                   OUTNUM )
            I = MEMUSED + 1
            CALL BTUNPACK( 'COMB', MEM, MEMUSED, MEM(I+1), MEM(I+2),
     $                     MEM(I+22), MEM(I+23), MEM(I+12), MEM(I+20),
     $                     MEM(I+3), MEM(I+13), NGRID, MEM(I+4),
     $                     MEM(I+14), MEM(I+21), MEM(I+5), MEM(I+15),
     $                     MEM(I+6), MEM(I+16), MEM(I+7), MEM(I+17),
     $                     MEM(I+8), MEM(I+18), MEM(I+9), MEM(I+19),
     $                     MEM(I+11), PPTR, QPTR )
*
            DO 30 I = 0, NGRID-1
               NNODES = MAX0( MEM(PPTR+I) * MEM(QPTR+I), NNODES )
   30       CONTINUE
         END IF
      END IF
*
*     If we run auxiliary tests, must have at least two nodes,
*     otherwise, minimum is 1
*
      IF( TESTAUX ) THEN
         NNODES = MAX0( NNODES, 2 )
      ELSE
         NNODES = MAX0( NNODES, 1 )
      END IF
*
      CALL BLACS_SETUP( IAM, NNODES )
*
*     We've buried a PNUM array in the common block above, and here
*     we initialize it.  The reason for carrying this along is so that
*     the TSEND and TRECV subroutines can report test results back to
*     the first process, which can then be the sole process
*     writing output files.
*
      CALL BLACS_GET( 0, 0, CONTEXT )
      CALL BLACS_GRIDINIT( CONTEXT, 'r', 1, NNODES )
*
      DO 40 I = 0, NNODES-1
         BTPNUMS(I) = BLACS_PNUM( CONTEXT, 0, I )
   40 CONTINUE
*
      CALL BLACS_GRIDEXIT( CONTEXT )
*
      RETURN
*
*     End of BTSETUP.
*
      END
*
      INTEGER FUNCTION IBTMYPROC()
*
*  -- BLACS tester (version 1.0) --
*  University of Tennessee
*  December 15, 1994
*
*  Purpose
*  =======
*  IBTMYPROC: returns a process number between 0 .. NPROCS-1.  On
*  systems not natively in this numbering scheme, translates to it.
*
*  ====================================================================
*
*     .. External Functions ..
      INTEGER  IBTNPROCS
      EXTERNAL IBTNPROCS
*     ..
*     .. Common blocks ..
      COMMON /BTPNUM/ BTPNUMS
*     ..
*     .. Arrays in Common ..
      INTEGER BTPNUMS(0:999)
*     ..
*     .. Local Scalars ..
      INTEGER IAM, I, K
*     ..
*     .. Save statement ..
      SAVE IAM
*     ..
*     .. Data statements ..
      DATA IAM /-1/
*     ..
*     .. Executable Statements ..
*
      IF (IAM .EQ. -1) THEN
         CALL PVMFMYTID(K)
         DO 10 I = 0, IBTNPROCS()-1
            IF( K .EQ. BTPNUMS(I) ) IAM = I
   10    CONTINUE
      END IF
*
      IBTMYPROC = IAM
      RETURN
*
*     End of IBTMYPROC
*
      END
*
      INTEGER FUNCTION IBTNPROCS()
*
*  -- BLACS tester (version 1.0) --
*  University of Tennessee
*  December 15, 1994
*
*  Purpose
*  =======
*  IBTNPROCS: returns the number of processes in the machine.
*
*  ====================================================================
*     .. Local Scalars ..
      INTEGER IAM, NNODES
*     ..
*
*     Got to use BLACS, since it set up the machine . . .
*
      CALL BLACS_PINFO(IAM, NNODES)
      IBTNPROCS = NNODES
*
      RETURN
*
*     End of IBTNPROCS
*
      END
*
      SUBROUTINE BTSEND(DTYPE, N, BUFF, DEST, MSGID)
*
*  -- BLACS tester (version 1.0) --
*  University of Tennessee
*  December 15, 1994
*
*     .. Scalar Arguments ..
      INTEGER N, DTYPE, DEST, MSGID
*     ..
*     .. Array Arguments ..
      REAL BUFF(*)
*     ..
*
*     PURPOSE
*     =======
*     BTSEND: Communication primitive used to send messages independent
*     of the BLACS.  May safely be either locally or globally blocking.
*
*     Arguments
*     =========
*     DTYPE    (input) INTEGER
*              Indicates what data type BUFF is (same as PVM):
*                1  =  RAW BYTES
*                3  =  INTEGER
*                4  =  SINGLE PRECISION REAL
*                6  =  DOUBLE PRECISION REAL
*                5  =  SINGLE PRECISION COMPLEX
*                7  =  DOUBLE PRECISION COMPLEX
*
*     N        (input) INTEGER
*              The number of elements of type DTYPE in BUFF.
*
*     BUFF     (input) accepted as INTEGER array
*              The array to be communicated.  Its true data type is
*              indicated by DTYPE.
*
*     DEST      (input) INTEGER
*               The destination of the message.
*
*     MSGID     (input) INTEGER
*               The message ID (AKA message tag or type).
*
* =====================================================================
*     .. External Functions ..
      INTEGER  IBTNPROCS
      EXTERNAL IBTNPROCS
*     ..
*     .. Common blocks ..
      COMMON /BTPNUM/ BTPNUMS
*     ..
*     .. Arrays in Common ..
      INTEGER BTPNUMS(0:999)
*     ..
*     .. Include Files ..
      INCLUDE 'fpvm3.h'
*     ..
*     .. Local Scalars ..
      INTEGER INFO, PVMTYPE
*     ..
*     .. Executable Statements ..
*
*     Map internal type parameters to PVM
*
      IF( DTYPE .EQ. 1 ) THEN
         PVMTYPE = BYTE1
      ELSE IF( DTYPE .EQ. 3 ) THEN
         PVMTYPE = INTEGER4
      ELSE IF( DTYPE .EQ. 4 ) THEN
         PVMTYPE = REAL4
      ELSE IF( DTYPE .EQ. 5 ) THEN
         PVMTYPE = COMPLEX8
      ELSE IF( DTYPE .EQ. 6 ) THEN
         PVMTYPE = REAL8
      ELSE IF( DTYPE .EQ. 7 ) THEN
         PVMTYPE = COMPLEX16
      END IF
*
*     pack and send data to specified process
*
      CALL PVMFINITSEND(PVMDATADEFAULT, INFO)
      CALL PVMFPACK(DTYPE, BUFF, N, 1, INFO)
      IF( DEST .EQ. -1 ) THEN
         CALL PVMFMCAST(IBTNPROCS(), BTPNUMS, MSGID, INFO)
      ELSE
         CALL PVMFSEND(BTPNUMS(DEST) , MSGID, INFO)
      ENDIF
*
      RETURN
*
*     End BTSEND
*
      END
*
      SUBROUTINE BTRECV(DTYPE, N, BUFF, SRC, MSGID)
*
*  -- BLACS tester (version 1.0) --
*  University of Tennessee
*  December 15, 1994
*
*
*     .. Scalar Arguments ..
      INTEGER N, DTYPE, SRC, MSGID
*     ..
*     .. Array Arguments ..
      REAL BUFF(*)
*     ..
*
*     PURPOSE
*     =======
*     BTRECV: Globally blocking receive.
*
*     Arguments
*     =========
*     DTYPE    (input) INTEGER
*              Indicates what data type BUFF is:
*                1  =  RAW BYTES
*                3  =  INTEGER
*                4  =  SINGLE PRECISION REAL
*                6  =  DOUBLE PRECISION REAL
*                5  =  SINGLE PRECISION COMPLEX
*                7  =  DOUBLE PRECISION COMPLEX
*
*     N        (input) INTEGER
*              The number of elements of type DTYPE in BUFF.
*
*     BUFF     (output) INTEGER
*              The buffer to receive into.
*
*     SRC      (input) INTEGER
*              The source of the message.
*
*     MSGID    (input) INTEGER
*              The message ID.
*
* =====================================================================
*
*     .. Common blocks ..
      COMMON /BTPNUM/ BTPNUMS
*     ..
*     .. Arrays in Common ..
      INTEGER BTPNUMS(0:999)
*     ..
*     .. Include Files ..
      INCLUDE 'fpvm3.h'
*     ..
*     .. Local Scalars ..
      INTEGER INFO, PVMTYPE
*     ..
*     .. Executable Statements ..
*
*     Map internal type parameters to PVM
*
      IF( DTYPE .EQ. 1 ) THEN
         PVMTYPE = BYTE1
      ELSE IF( DTYPE .EQ. 3 ) THEN
         PVMTYPE = INTEGER4
      ELSE IF( DTYPE .EQ. 4 ) THEN
         PVMTYPE = REAL4
      ELSE IF( DTYPE .EQ. 5 ) THEN
         PVMTYPE = COMPLEX8
      ELSE IF( DTYPE .EQ. 6 ) THEN
         PVMTYPE = REAL8
      ELSE IF( DTYPE .EQ. 7 ) THEN
         PVMTYPE = COMPLEX16
      END IF
      CALL PVMFRECV(BTPNUMS(SRC), MSGID, INFO)
      CALL PVMFUNPACK(DTYPE, BUFF, N, 1, INFO)
*     ..
*     .. Local Scalars ..
*
      RETURN
*
*     End of BTRECV
*
      END
*
      INTEGER FUNCTION IBTSIZEOF(TYPE)
*
*  -- BLACS tester (version 1.0) --
*  University of Tennessee
*  December 15, 1994
*
*     .. Scalar Arguments ..
      CHARACTER*1 TYPE
*     ..
*
*  Purpose
*  =======
*  IBTSIZEOF: Returns the size, in bytes, of the 5 data types.
*  If your platform has a different size for DOUBLE PRECISION, you must
*  change the parameter statement in BLACSTEST as well.
*
*  Arguments
*  =========
*  TYPE     (input) CHARACTER*1
*           The data type who's size is to be determined:
*           'I' : INTEGER
*           'S' : SINGLE PRECISION REAL
*           'D' : DOUBLE PRECISION REAL
*           'C' : SINGLE PRECISION COMPLEX
*           'Z' : DOUBLE PRECISION COMPLEX
*
* =====================================================================
*
*     .. External Functions ..
      LOGICAL  LSAME
      EXTERNAL LSAME
*     ..
*     .. Local Scalars ..
      INTEGER LENGTH
*     ..
*     .. Executable Statements ..
*
      IF( LSAME(TYPE, 'I') ) THEN
         LENGTH = 4
      ELSE IF( LSAME(TYPE, 'S') ) THEN
         LENGTH = 4
      ELSE IF( LSAME(TYPE, 'D') ) THEN
         LENGTH = 8
      ELSE IF( LSAME(TYPE, 'C') ) THEN
         LENGTH = 8
      ELSE IF( LSAME(TYPE, 'Z') ) THEN
         LENGTH = 16
      END IF
      IBTSIZEOF = LENGTH
*
      RETURN
      END
