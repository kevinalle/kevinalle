#include<stdio.h>
#include<mpi.h>
#include<cstdlib>

extern "C"{
	extern void   Cblacs_pinfo( int* mypnum, int* nprocs);
	extern void   Cblacs_get( int context, int request, int* value);
	extern int    Cblacs_gridinit( int* context, char order, int np_row, int np_col);
	extern void   Cblacs_gridinfo( int context, int*  np_row, int* np_col, int*  my_row, int*  my_col);
	extern void   Cblacs_gridexit( int context);
	extern void   Cblacs_exit( int error_code);
}

void err(const char*errlog){
	printf("%s\n",errlog);
	exit(0);
}

int main(int argc,char**argv){
	
	int rank_mpi,rank_blacs;
	int np_mpi,np_blacs;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank_mpi);
	MPI_Comm_size(MPI_COMM_WORLD, &np_mpi);
	
	Cblacs_pinfo(&rank_blacs,&np_blacs);
	if(np_mpi!=9 || np_blacs!=9) err("must have 9 proc");
	int ictxt;
	Cblacs_get(-1, 0, &ictxt);
	int npcol=3;
	int nprow=3;
	Cblacs_gridinit(&ictxt, 'R', nprow, npcol);
	int myrow,mycol;
	Cblacs_gridinfo(ictxt, &nprow, &npcol, &myrow, &mycol);
	
	return 0;
}
