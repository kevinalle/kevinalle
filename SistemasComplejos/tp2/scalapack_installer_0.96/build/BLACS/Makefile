help :
	@ echo
	@ echo "Make sure you are using the correct Bmake.inc for your system."
	@ echo "At this level, assuming you have downloaded the necessary files,"
	@ echo "you may make the BLACS tester (make tester), or one of the BLACS"
	@ echo "versions (make cmmd, make mpl, make nx, or make pvm)"
	@ echo "You can define the make macro 'what' to perform a specific action."
	@ echo "(eg., make tester what=clean)"
	@ echo "There are short README files in TESTING/ and SRC/."
	@ echo

all : mpi cmmd mpl nx pvm tester

cleanall:
	( cd TESTING ; make clean )
	( cd SRC/CMMD ; make clean )
	( cd SRC/MPL  ; make clean )
	( cd SRC/NX   ; make clean )
	( cd SRC/PVM  ; make clean )

testing: tester
xbtest : tester
test   : tester
tester :
	( cd TESTING ; make $(what) )

CM5  : CMMD
cmmd : CMMD
CMMD :
	( cd SRC/CMMD ; make $(what) )

SP1 : MPL
SP2 : MPL
mpl : MPL
MPL :
	( cd SRC/MPL ; make $(what) )

intel   : NX
ipsc2   : NX
i860    : NX
delta   : NX
gamma   : NX
paragon : NX
nx      : NX
NX :
	( cd SRC/NX ; make $(what) )

pvm : PVM
PVM :
	( cd SRC/PVM ; make $(what) )

mpi : MPI
MPI :
	( cd SRC/MPI ; make $(what) )
