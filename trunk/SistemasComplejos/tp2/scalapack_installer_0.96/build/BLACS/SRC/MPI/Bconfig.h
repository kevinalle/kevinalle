/*
 *  This file includes the standard C libraries, as well as system dependant
 *  include files.  All BLACS routines include this file.
 */

#ifndef BCONFIG_H
#define BCONFIG_H 1

/*
 * Include files
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifdef __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <mpi.h>

/*
 * These macros define the naming strategy needed for a fortran77
 * routine to call a C routine, and whether to build so they may be
 * called from C or f77.  For the f77 call C interface, ADD_ assumes that
 * f77 calls expect C routines to have an underscore postfixed to the name
 * (Suns, and the Intel expect this).  NOCHANGE indicates that f77 expects
 * the name called by fortran to be identical to that compiled by C
 * (RS6K's do this).  UPCASE says it expects C routines called by fortran
 * to be in all upcase (CRAY wants this).  The variable F77_CALL_C is always
 * set to one of these values.  If the BLACS will be called from C, we define
 * INTFACE to be CALL_C, otherwise, it is set to F77_CALL_C.
 */
#define ADD_     0
#define NOCHANGE 1
#define UPCASE   2
#define F77ISF2C 3
#define C_CALL   4

#ifdef UpCase
#define F77_CALL_C UPCASE
#endif

#ifdef NoChange
#define F77_CALL_C NOCHANGE
#endif

#ifdef Add_
#define F77_CALL_C ADD_
#endif

#ifdef f77IsF2C
#define F77_CALL_C F77ISF2C
#endif

#ifndef F77_CALL_C
#define F77_CALL_C ADD_
#endif

#ifdef CallFromC
#define INTFACE C_CALL
#else
#define INTFACE F77_CALL_C
#endif

/*
 *  Uncomment these macro definitions, and substitute the topology of your
 *  choice to vary the default topology (TOP = ' ') for broadcast and combines.
#define DefBSTop '1'
#define DefCombTop '1'
 */

/*
 * Uncomment this line if your MPI_Send provides a locally-blocking send
 */
/* #define SndIsLocBlk */

/*
 * Comment out the following line if your MPI does a data copy on every
 * non-contiguous send
 */
#ifndef NoMpiBuff
#define MpiBuffGood
#endif

/*
 * If your MPI cannot form data types of zero length, uncomment the
 * following definition
 */
/* #define ZeroByteTypeBug */

/*
 * Figure out how to translate between C and fortran communicators.
 */
#define USEMPICH 1
#define CSAMEF77 2
#define BONEHEAD 3
#define USEMPI2  4

#ifdef UseMpi2
#define BI_TransComm USEMPI2
#endif

#ifdef UseMpich
#define BI_TransComm USEMPICH
#endif

#ifdef CSameF77
#define BI_TransComm CSAMEF77
#endif

#ifndef BI_TransComm
#define BI_TransComm BONEHEAD
#endif

/*
 * If the user has not chosen which MPI interface to use, use F77 if
 * BLACS_GRIDMAP/INIT is going to block, and C otherwise
 */
#ifndef UseCMpi
#ifndef UseF77Mpi

#if (BI_TransComm == BONEHEAD)
#define UseF77Mpi
#endif

#endif
#endif

/*
 *  These macros set the timing and debug levels for the BLACS.  The fastest
 *  code is produced by setting both values to 0.  Higher levels provide
 *  more timing/debug information at the cost of performance.  Present levels
 *  of debug are:
 *  0 : No debug information
 *  1 : Mainly parameter checking.
 *
 *  Present levels of timing are:
 *  0 : No timings taken
 */
#ifndef BlacsDebugLvl
#define BlacsDebugLvl 0
#endif
#ifndef BlacsTimingLvl
#define BlacsTimingLvl 0
#endif

#include "Bdef.h"
#endif
