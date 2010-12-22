distrib: distrib.c
	mpiCC -o distrib distrib.c -lgfortran ./scalapack_installer_0.96/lib/libscalapack.a ./scalapack_installer_0.96/lib/librefblas.a ./scalapack_installer_0.96/lib/blacs.a ./scalapack_installer_0.96/lib/blacsC.a ./scalapack_installer_0.96/lib/blacsF77.a ./scalapack_installer_0.96/lib/blacs.a

run: distrib
	mpirun -np 12 distrib < matrix.in
