#include<iostream>
#include<mpi.h>
#include<cstdlib>
#include<math.h>
using namespace std;

#define forn(i,n) for(int i=0;i<n;i++)
#define forsn(i,s,n) for(int i=(int)(s);i<n;i++)

int main(int argc, char**argv){
	MPI_Init(&argc,&argv);
	int p,r;
	MPI_Comm_rank(MPI_COMM_WORLD,&r);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	char rootname[50];
	if(r==0) gethostname(rootname,50);
	if(r==0) printf("%s es el root :)\n",rootname);
	MPI_Bcast(rootname,50,MPI_CHAR,0,MPI_COMM_WORLD);
	if(r!=0){//soy esclavo
		char myname[50];
		gethostname(myname,50);
		char mensaje[255];
		sprintf(mensaje,"hola %s!, soy %s con rank %d",rootname,myname,r);
		MPI_Send(&mensaje,255,MPI_CHAR,0,0,MPI_COMM_WORLD);
	}else{//soy director
		char buff[255];
		MPI_Status s;
		int i;
		for(i=0;i<p-1;i++){
			MPI_Recv(&buff,255,MPI_CHAR,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,&s);
			clog<<buff<<endl;
		}
	}
	MPI_Finalize();
	return 0;
}
