
SHELL         = /bin/sh

home          = /home/kevin/UBA/SistemasComplejos/tp2/scalapack_installer_0.96/build/scalapack-1.8.0

PLAT          = LINUX

USEMPI        = -DUsingMpiBlacs

SMPLIB        = 
BLACSFINIT    = /home/kevin/UBA/SistemasComplejos/tp2/scalapack_installer_0.96/lib/blacsF77.a 
BLACSCINIT    = /home/kevin/UBA/SistemasComplejos/tp2/scalapack_installer_0.96/lib/blacsC.a 
BLACSLIB      = /home/kevin/UBA/SistemasComplejos/tp2/scalapack_installer_0.96/lib/blacs.a 
TESTINGdir    = $(home)/TESTING


CBLACSLIB     = $(BLACSCINIT) $(BLACSLIB) $(BLACSCINIT)
FBLACSLIB     = $(BLACSFINIT) $(BLACSLIB) $(BLACSFINIT)

PBLASdir      = $(home)/PBLAS
SRCdir        = $(home)/SRC
TESTdir       = $(home)/TESTING
PBLASTSTdir   = $(TESTINGdir)
TOOLSdir      = $(home)/TOOLS
REDISTdir     = $(home)/REDIST
REDISTTSTdir  = $(TESTINGdir)

F77           = /usr/bin/mpif77
CC            = /usr/bin/mpicc
NOOPT         = 
F77FLAGS      =  $(NOOPT) -O3
CCFLAGS       = -O3
SRCFLAG       =
F77LOADER     = $(F77) 
CCLOADER      = $(F77)
F77LOADFLAGS  = 
CCLOADFLAGS   = 

CDEFS         = -DNO_IEEE $(USEMPI) -DAdd_

ARCH          = ar
ARCHFLAGS     = cr
RANLIB        = /usr/bin/ranlib

SCALAPACKLIB  = $(home)/libscalapack.a
BLASLIB       = /usr/lib/libblas.so.3gf
LAPACKLIB     = /usr/lib/liblapack.so.3gf

PBLIBS        = $(SCALAPACKLIB) $(FBLACSLIB) $(LAPACKLIB) $(BLASLIB) $(SMPLIB)
PRLIBS        = $(SCALAPACKLIB) $(CBLACSLIB) $(SMPLIB)
RLIBS         = $(SCALAPACKLIB) $(FBLACSLIB) $(CBLACSLIB) $(LAPACKLIB) $(BLASLIB) $(SMPLIB)
LIBS          = $(PBLIBS)
        