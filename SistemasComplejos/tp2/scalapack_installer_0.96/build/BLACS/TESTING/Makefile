include ../Bmake.inc

#  ---------------------------------------------------------------------
#  The file tools.f contains some LAPACK routines that the tester calls.
#  If you have ScaLAPACK, you may point to your tools library instead
#  of compiling this file.
#  ---------------------------------------------------------------------
   tools = tools.o

exe : all
ctest : $(CTESTexe) dat
ftest : $(FTESTexe) dat
all : $(FTESTexe) $(CTESTexe) dat
dat : $(TESTdir)/bt.dat $(TESTdir)/sdrv.dat $(TESTdir)/bsbr.dat \
      $(TESTdir)/comb.dat

obj = blacstest.o btprim_$(COMMLIB).o

$(CTESTexe): $(obj) $(tools)
	$(CC) -c $(CCFLAGS) -DBTCINTFACE $(BLACSDEFS) Cbt.c
	$(F77LOADER) $(F77LOADFLAGS) -o $@ $(obj) $(tools) Cbt.o $(BTLIBS)

$(FTESTexe): $(obj) $(tools)
	$(F77LOADER) $(F77LOADFLAGS) -o $@ $(obj) $(tools) $(BTLIBS)

#  --------------------------------------------------------------------
#  The files tools.f and blacstest.f are compiled without optimization.
#  Tools.f contains the LAPACK routines slamch and dlamch, which only
#  operate correctly for low-levels of optimization.  Blacstest.f is
#  extremely large, and optimizing it takes a long time.  More
#  importantly, the sun's f77 compiler seems to produce errors in
#  trying to optimize such a large file.  We therefore insist that it
#  also not be optimized.
#  --------------------------------------------------------------------
tools.o : tools.f
	$(F77) $(F77NO_OPTFLAGS) -c $*.f

blacstest.o : blacstest.f
	$(F77) $(F77NO_OPTFLAGS) -c $*.f

$(TESTdir)/bt.dat : $(BTOPdir)/TESTING/bt.dat
	cp $(BTOPdir)/TESTING/bt.dat $(TESTdir)/

$(TESTdir)/sdrv.dat : $(BTOPdir)/TESTING/sdrv.dat
	cp $(BTOPdir)/TESTING/sdrv.dat $(TESTdir)/

$(TESTdir)/bsbr.dat : $(BTOPdir)/TESTING/bsbr.dat
	cp $(BTOPdir)/TESTING/bsbr.dat $(TESTdir)/

$(TESTdir)/comb.dat : $(BTOPdir)/TESTING/comb.dat
	cp $(BTOPdir)/TESTING/comb.dat $(TESTdir)/

btprim_MPI.o : btprim_MPI.f
	make mpif.h
	$(F77) -c $(F77FLAGS) $*.f

btprim_PVM.o : btprim_PVM.f
	make fpvm3.h
	$(F77) -c $(F77FLAGS) $*.f

mpif.h : $(MPIINCdir)/mpif.h
	rm -f mpif.h
	ln -s $(MPIINCdir)/mpif.h mpif.h

fpvm3.h : $(PVMINCdir)/fpvm3.h
	rm -f fpvm3.h
	ln -s $(PVMINCdir)/fpvm3.h fpvm3.h

clean :
	rm -f $(obj) tools.o Cbt.o mpif.h fpvm3.h

.f.o: ; $(F77) -c $(F77FLAGS) $*.f
