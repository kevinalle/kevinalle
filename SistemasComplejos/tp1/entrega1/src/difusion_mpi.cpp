#include<iostream>
#include<mpi.h>
#include<cstdlib>
#include<math.h>
using namespace std;

#define forn(i,n) for(int i=0;i<n;i++)
#define forsn(i,s,n) for(int i=(int)(s);i<n;i++)

double F(double x){return x<.5?10.:-5.; /*return 0;*/}
double LeftBorder(double t){return 10.;}
double RightBorder(double t){return -5.;}

int main(int argc, char**argv){
	double dt=.000025;
	double dx=.01;
	double K=1.; // Coeficiente de difusion termica adimensionalizado
	double L=1.; // Longitud
	double r=dt*K/(dx*dx);
	
	int output_t_samples=20;
	int output_x_samples=100;
	
	double tf=.1;
	
	int times=tf/dt+1;
	int samples=L/dx+2;
	
	
	MPI_Init(&argc,&argv);
	int np,rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&np);
	
	if(rank!=0){
		// creo mis arrays
		int my_start=(rank-1)*samples/(np-1);
		int my_end=rank==np-1?samples:rank*samples/(np-1);
		int my_size=my_end-my_start;
		clog<<"worker "<<rank<<" computando desde "<<my_start<<" hasta "<<my_end<<" ("<<my_size<<")"<<endl;
		double*u=new double[my_size+2];
		double*u2=new double[my_size+2];
		forsn(i,my_start,my_end+2) u[i-my_start]=F((double)i/samples);
		
		u[0]=rank==1?LeftBorder(0.):F((double)(my_start-1)/samples);
		u[my_size+1]=rank==np-1?RightBorder(0.):F((double)(my_end+1)/samples);
		
		forn(i,my_size+2) u2[i]=u[i];
		
		MPI_Status s;
		if(rank!=1) MPI_Send(u+1,1,MPI_DOUBLE,rank-1,1,MPI_COMM_WORLD);
		if(rank!=np-1) MPI_Send(u+my_size,1,MPI_DOUBLE,rank+1,1,MPI_COMM_WORLD);
		forn(t,times+1){
			if(rank!=1) MPI_Recv(u2,1,MPI_DOUBLE,rank-1,1,MPI_COMM_WORLD,&s);
			else u2[0]=LeftBorder(tf*t/times);
			if(rank!=np-1) MPI_Recv(u2+my_size+1,1,MPI_DOUBLE,rank+1,1,MPI_COMM_WORLD,&s);
			else u2[my_size+1]=RightBorder(tf*t/times);
			forn(iter,20){
				forsn(i,1,my_size+1){
					//u2[i]=r*u[i+1]+(1-2*r)*u[i]+r*u[i-1];
					u2[i]=(r/(1+2*r))*(u2[i+1]+u2[i-1])+u[i]/(1+2*r);
				}
			}
			double*temp=u; u=u2; u2=temp;
			if(rank!=1) MPI_Send(u+1,1,MPI_DOUBLE,rank-1,1,MPI_COMM_WORLD);
			if(rank!=np-1) MPI_Send(u+my_size,1,MPI_DOUBLE,rank+1,1,MPI_COMM_WORLD);
		
			if(t%(times/output_t_samples)==0){
				clog<<"worker "<<rank<<" enviando t="<<t<<endl;
				MPI_Send(u+1,my_size,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
			}
		}
	}else{
		//recibir resultados
		double*u=new double[samples];
		MPI_Status s;
		forn(t,(times+1)/(times/output_t_samples)+1){
			forsn(i,1,np) MPI_Recv(u+(i-1)*samples/(np-1), (i==np-1?samples:i*samples/(np-1))-((i-1)*samples/(np-1)), MPI_DOUBLE,i,0,MPI_COMM_WORLD,&s);
			for(int i=0;i<samples;i+=samples/output_x_samples) cout << u[i] << " ";
			cout<<endl;
		}
		clog<<"root ok"<<endl;
	}
	MPI_Finalize();
	return 0;
}
