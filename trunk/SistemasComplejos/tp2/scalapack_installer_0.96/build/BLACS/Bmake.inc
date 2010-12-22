
SHELL = /bin/sh
BTOPdir = /home/kevin/UBA/SistemasComplejos/tp2/scalapack_installer_0.96/build/BLACS
COMMLIB = MPI
PLAT = 
BLACSdir    = $(BTOPdir)/LIB
BLACSDBGLVL = 0
BLACSFINIT  = $(BLACSdir)/blacsF77.a
BLACSCINIT  = $(BLACSdir)/blacsC.a
BLACSLIB    = $(BLACSdir)/blacs.a

MPIINCdir = /usr/lib/openmpi/include
MPILIB = 

BTLIBS = $(BLACSFINIT) $(BLACSLIB) $(BLACSFINIT) $(MPILIB)
INSTdir = $(BTOPdir)/INSTALL/EXE
TESTdir = $(BTOPdir)/TESTING/EXE
FTESTexe = $(TESTdir)/xFbtest_$(COMMLIB)-$(PLAT)-$(BLACSDBGLVL)
CTESTexe = $(TESTdir)/xCbtest_$(COMMLIB)-$(PLAT)-$(BLACSDBGLVL)
SYSINC = -I$(MPIINCdir)
INTFACE = -DAdd_
SENDIS = 
BUF = 
TRANSCOMM =  -DUseMpi2
WHATMPI = 
SYSERRORS = 
DEBUGLVL = -DBlacsDebugLvl=$(BLACSDBGLVL)
DEFS1 = -DSYSINC $(SYSINC) $(INTFACE) $(DEFBSTOP) $(DEFCOMBTOP) $(DEBUGLVL)
BLACSDEFS = $(DEFS1) $(SENDIS) $(BUFF) $(TRANSCOMM) $(WHATMPI) $(SYSERRORS)

F77            = /usr/bin/mpif77
F77NO_OPTFLAGS = 
F77FLAGS       = $(F77NO_OPTFLAGS) -O3
F77LOADER      = $(F77)
F77LOADFLAGS   = 
CC             = /usr/bin/mpicc
CCFLAGS        = -O3
CCLOADER       = $(F77)
CCLOADFLAGS    = 
ARCH      = ar
ARCHFLAGS = r
RANLIB    = /usr/bin/ranlib
      