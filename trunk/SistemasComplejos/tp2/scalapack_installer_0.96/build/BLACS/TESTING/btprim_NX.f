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
*  BTSETUP:  Does nothing on non-PVM platforms
*
*  ====================================================================
*     .. Executable Statements ..
      RETURN
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
      INTEGER  MYNODE
      EXTERNAL MYNODE
*     ..
*     .. Executable Statements ..
*
      IBTMYPROC = MYNODE()
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
*     .. External Functions ..
      INTEGER  NUMNODES
      EXTERNAL NUMNODES
*     ..
*     .. Executable Statements ..
*
      IBTNPROCS = NUMNODES()
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
      INTEGER  IBTSIZEOF
      EXTERNAL IBTSIZEOF
*     ..
*     .. Local Scalars ..
      INTEGER LENGTH
      INTEGER ISIZE, SSIZE, DSIZE, CSIZE, ZSIZE
*     ..
*     .. Save statement ..
      SAVE  ISIZE, SSIZE, DSIZE, CSIZE, ZSIZE
*     ..
*     .. Data statements ..
      DATA  ISIZE /-50/
*     ..
*     .. Executable Statements ..
*
*     On first call, initialize size variables
*
      IF( ISIZE .LT. 0 ) THEN
         ISIZE = IBTSIZEOF('I')
         SSIZE = IBTSIZEOF('S')
         DSIZE = IBTSIZEOF('D')
         CSIZE = IBTSIZEOF('C')
         ZSIZE = IBTSIZEOF('Z')
      END IF
*
*     Figure length of buffer
*
      IF( DTYPE .EQ. 1 ) THEN
         LENGTH = N
      ELSE IF( DTYPE .EQ. 3 ) THEN
         LENGTH = N * ISIZE
      ELSE IF( DTYPE .EQ. 4 ) THEN
         LENGTH = N * SSIZE
      ELSE IF( DTYPE .EQ. 5 ) THEN
         LENGTH = N * CSIZE
      ELSE IF( DTYPE .EQ. 6 ) THEN
         LENGTH = N * DSIZE
      ELSE IF( DTYPE .EQ. 7 ) THEN
         LENGTH = N * ZSIZE
      END IF
*
*     Send the message
*
      CALL CSEND(MSGID, BUFF, LENGTH, DEST, 0)
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
*     .. External Functions ..
      INTEGER  IBTSIZEOF
      EXTERNAL IBTSIZEOF
*     ..
*     .. Local Scalars ..
      INTEGER LENGTH
      INTEGER ISIZE, SSIZE, DSIZE, CSIZE, ZSIZE
*     ..
*     .. Save statement ..
      SAVE  ISIZE, SSIZE, DSIZE, CSIZE, ZSIZE
*     ..
*     .. Data statements ..
      DATA  ISIZE /-50/
*     ..
*     .. Executable Statements ..
*
*     On first call, initialize size variables
*
      IF( ISIZE .LT. 0 ) THEN
         ISIZE = IBTSIZEOF('I')
         SSIZE = IBTSIZEOF('S')
         DSIZE = IBTSIZEOF('D')
         CSIZE = IBTSIZEOF('C')
         ZSIZE = IBTSIZEOF('Z')
      END IF
*
*     Figure length of buffer
*
      IF( DTYPE .EQ. 1 ) THEN
         LENGTH = N
      ELSE IF( DTYPE .EQ. 3 ) THEN
         LENGTH = N * ISIZE
      ELSE IF( DTYPE .EQ. 4 ) THEN
         LENGTH = N * SSIZE
      ELSE IF( DTYPE .EQ. 5 ) THEN
         LENGTH = N * CSIZE
      ELSE IF( DTYPE .EQ. 6 ) THEN
         LENGTH = N * DSIZE
      ELSE IF( DTYPE .EQ. 7 ) THEN
         LENGTH = N * ZSIZE
      END IF
*
*     Receive the message
*
      CALL CRECV(MSGID, BUFF, LENGTH)
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
