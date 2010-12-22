#include<stdio.h>
#include<mpi.h>
#include<cstdlib>
#include<cmath>

#define forn(i,n) for(int i=0;i<(n);i++)

int blockrows=2;
int blockcols=2;

extern "C"{
	extern void Cblacs_pinfo( int* mypnum, int* nprocs);
	extern void Cblacs_get( int context, int request, int* value);
	extern int  Cblacs_gridinit( int* context, char* order, int np_row, int np_col);
	extern void Cblacs_gridinfo( int context, int*  np_row, int* np_col, int*  my_row, int*  my_col);
	extern void Cblacs_gridexit( int context);
	extern void Cblacs_exit(int error_code);
	extern void Cblacs_barrier(int icontxt, const char*scope);
	extern void Cdgesd2d(int icontxt, int m, int n, double*A, int lda, int rdest, int cdest);
	extern void Cdgerv2d(int icontxt, int m, int n, double*A, int lda, int rsrc, int csrc);
	extern void Cigebs2d(int icontxt, const char*scope, const char*top, int m, int n, int*A, int lda);
	extern void Cigebr2d(int icontxt, const char*scope, const char*top, int m, int n, int*A, int lda, int rsrc, int csrc);
	extern void descinit_(int* desc, int*m, int*n, int*mb, int*nb, int*rsrc, int*csrc, int*icontxt, int*lld, int*ierr);
	extern int  numroc_(int*m, int*mb, int*myrow, int*rsrc, int*nprow);
	extern void pdgesv_(int*n , int*NRHS, double*A, int*IA, int*JA, int*DESCA, int*IPIV, double*B, int*IB,  int*JB,  int*DESCB, int*INFO);
}

void err(const char*errlog){
	printf("[ERROR] %s\n",errlog);
	exit(0);
}

void matrixprint(double*m,int h,int w){
	forn(i,h){
		forn(j,w) printf("%.1f\t",m[i*w+j]);
		printf("\n");
	}
}

void subsize(int rows, int cols, int nprows, int npcols, int pi, int pj, int*subrows, int*subcols){
	int nbrows=rows/blockrows;
	int nbcols=cols/blockcols;
	if(nprows>nbrows||npcols>nbcols) err("mas procesos que bloques");
	int subblockrows=(nbrows/nprows) + (pi<nbrows%nprows);
	int subblockcols=(nbcols/npcols) + (pj<nbcols%npcols);
	int extrarows=(pi==nbrows%nprows)*(rows%blockrows);
	int extracols=(pj==nbcols%npcols)*(cols%blockcols);
	*subrows=subblockrows*blockrows+extrarows;
	*subcols=subblockcols*blockcols+extracols;
}

void distribute(double*matrix,int rows,int cols,int nprows,int npcols,int ictxt){
	int nbrows=rows/blockrows;
	int nbcols=cols/blockcols;
	if(nprows>nbrows||npcols>nbcols) err("mas procesos que bloques");
	forn(pi,nprows) forn(pj,npcols){
		int subblockrows=(nbrows/nprows) + (pi<nbrows%nprows);
		int subblockcols=(nbcols/npcols) + (pj<nbcols%npcols);
		int extrarows=(pi==nbrows%nprows)*(rows%blockrows);
		int extracols=(pj==nbcols%npcols)*(cols%blockcols);
		int subrows=subblockrows*blockrows+extrarows;
		int subcols=subblockcols*blockcols+extracols;
		printf("proceso %d %d: %dx%d = %dx%d blocks\n",pi,pj,subrows,subcols,subblockrows,subblockcols);
		double submatrix[subrows*subcols];
		forn(si,subblockrows+(extrarows>0))
		forn(sj,subblockcols+(extracols>0))
			forn(ii,si==subblockrows-1&&extrarows>0?extrarows:blockrows)
			forn(jj,sj==subblockcols-1&&extracols>0?extracols:blockcols)
				submatrix[(si*blockrows+ii)*subcols + (sj*blockcols+jj)]=matrix[(pi*blockrows+si*blockrows*nprows+ii)*cols + (pj*blockcols+sj*blockcols*npcols+jj)];
				//printf("submatrix[%d][%d]=matrix[%d][%d]\n",(si*blockrows+ii),(sj*blockcols+jj),(si*blockrows*nprows+ii),(sj*blockcols*npcols+jj));
		matrixprint(submatrix,subrows,subcols);
		Cdgesd2d(ictxt, subrows, subcols, submatrix, 1, pi, pj);
	}
/*	forn(i,rows){
		forn(j,cols) printf("%d\t",matrix[i*cols+j]);
		printf("\n");
	}*/
}

void readmatrix(int ictxt, int myrow, int mycol, int nprow, int npcol, double*submatrix, int*desc){
	int rows,cols;
	if(myrow==0&&mycol==0){
		printf("Workers grid: %dx%d\n",nprow,npcol);
		//printf("Sobran %d workers\n",np_mpi%npcol);
		scanf("%d",&rows);
		scanf("%d",&cols);
		printf("Input matrix %dx%d\n",rows,cols);
		double matrix[rows*cols];
		float scan;
		forn(i,rows*cols){
			scanf("%f",&scan);
			matrix[i]=scan;
		}
		matrixprint(matrix,rows,cols);
		Cigebs2d(ictxt,"All","i-ring",1,1,&rows,1);
		Cigebs2d(ictxt,"All","i-ring",1,1,&cols,1);
		distribute(matrix,rows,cols,nprow,npcol,ictxt);
	}else{
		Cigebr2d(ictxt,"All","i-ring",1,1,&rows,1,0,0);
		Cigebr2d(ictxt,"All","i-ring",1,1,&cols,1,0,0);
	}
	int subrows,subcols;
	subsize(rows,cols,nprow,npcol,myrow,mycol,&subrows,&subcols);
	//double submatrix[subrows*subcols];
	submatrix=(double*)malloc(subrows*subcols*sizeof(double));
	Cdgerv2d(ictxt, subrows, subcols, submatrix, 1, 0, 0);
	printf("Proceso %d,%d: recibi submatrix de %dx%d\n",myrow,mycol,subrows,subcols);
	
	//int A[9];
	desc=(int*)malloc(9*sizeof(int));
	int ierr;
	int rsrc=0;int csrc=0;
	int lld=numroc_(&cols, &blockcols, &myrow, &rsrc, &nprow);
	descinit_(desc,&rows,&cols,&blockrows,&blockcols,&rsrc,&csrc,&ictxt,&lld,&ierr);

	Cblacs_barrier(ictxt,"All");
	
	//pdgesv_(int*N , int*NRHS, double*A, int*IA, int*JA, int*DESCA, int*IPIV, double*B, int*IB,  int*JB,  int*DESCB, int*INFO);
}

int main(int argc,char**argv){
	
	int rank_mpi,rank_blacs;
	int np_mpi,np_blacs;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank_mpi);
	MPI_Comm_size(MPI_COMM_WORLD, &np_mpi);
	
	Cblacs_pinfo(&rank_blacs,&np_blacs);
	//if(np_mpi!=9 || np_blacs!=9) err("must have 9 proc");
	int ictxt;
	Cblacs_get(-1, 0, &ictxt);

	float gridratio=1.; //relacion alto/ancho de la grid deseada (machear la matriz?)
	int npcol=round(sqrt(gridratio*np_mpi));
	int nprow=np_mpi/npcol;
	char order='R';
	Cblacs_gridinit(&ictxt, &order, nprow, npcol);
	int myrow,mycol;
	Cblacs_gridinfo(ictxt, &nprow, &npcol, &myrow, &mycol);
	
	double*A;
	int*descA;
	readmatrix(ictxt,myrow,mycol,nprow,npcol,A,descA);
	
	double*B;
	int*descB;
	readmatrix(ictxt,myrow,mycol,nprow,npcol,B,descB);
	
	// Finalize
	Cblacs_gridexit(ictxt);
	Cblacs_exit(0);
	return 0;
}
