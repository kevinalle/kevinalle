#include<iostream>
#include<mpi.h>
#include<cstdlib>
#include<math.h>
using namespace std;

#define forn(i,n) for(int i=0;i<n;i++)
#define forsn(i,s,n) for(int i=(int)(s);i<n;i++)
int GHOST=2;

double F(double x){
	//return x<.5?10.:-5.;
	return 0;
}
double LeftBorder(double t){
	return 10.;
}
double RightBorder(double t){
	return -5.;
}

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
		double*u=new double[my_size+2*GHOST];
		double*u2=new double[my_size+2*GHOST];
		forn(i,2*GHOST) u[i]=F((double)(my_start+i-GHOST)/samples);
		
		forn(i,GHOST) if(rank==1) u[i]=LeftBorder(0.); else u[i]=F((double)(my_start-GHOST+i+1)/samples);
		forsn(i,my_size+GHOST-1,my_size+2*GHOST) if(rank==np-1) u[i]=RightBorder(0.); else u[i]=F((double)(my_start+i)/samples);
		
		forn(i,my_size+2*GHOST) u2[i]=u[i];
		
		MPI_Send(u+GHOST,my_size,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
		
		MPI_Status s;
		if(rank!=1) MPI_Send(u+GHOST,GHOST,MPI_DOUBLE,rank-1,1,MPI_COMM_WORLD);
		if(rank!=np-1) MPI_Send(u+my_size,GHOST,MPI_DOUBLE,rank+1,1,MPI_COMM_WORLD);
		forn(t,times+1){
			if(rank!=1) MPI_Recv(u2,GHOST,MPI_DOUBLE,rank-1,1,MPI_COMM_WORLD,&s);
			else forn(i,GHOST) u2[i]=LeftBorder(tf*t/times);
			if(rank!=np-1) MPI_Recv(u2+my_size+GHOST,GHOST,MPI_DOUBLE,rank+1,1,MPI_COMM_WORLD,&s);
			else forsn(i,my_size+GHOST,my_size+2*GHOST) u2[i]=RightBorder(tf*t/times);
			//forn(iter,20){
				forsn(i,GHOST,my_size+GHOST){
					u2[i]=r*u[i+1]+(1-2*r)*u[i]+r*u[i-1];
					//u2[i]=(r/(1+2*r))*(u2[i+1]+u2[i-1])+u[i]/(1+2*r);
				}
			//}
			double*temp=u; u=u2; u2=temp;
			if(rank!=1) MPI_Send(u+GHOST,GHOST,MPI_DOUBLE,rank-1,1,MPI_COMM_WORLD);
			if(rank!=np-1) MPI_Send(u+my_size,GHOST,MPI_DOUBLE,rank+1,1,MPI_COMM_WORLD);
		
			if(t%(times/output_t_samples)==0){
				clog<<"worker "<<rank<<" enviando t="<<t<<endl;
				MPI_Send(u+GHOST,my_size,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
			}
		}
	}else{
		//recibir resultados
		double*u=new double[samples];
		MPI_Status s;
		forn(t,(times+1)/(times/output_t_samples)+1+1){
			forsn(i,1,np) MPI_Recv(u+(i-1)*samples/(np-1), (i==np-1?samples:i*samples/(np-1))-((i-1)*samples/(np-1)), MPI_DOUBLE,i,0,MPI_COMM_WORLD,&s);
			for(int i=0;i<samples;i+=samples/output_x_samples) cout << u[i] << " ";
			cout<<endl;
		}
		clog<<"root ok"<<endl;
	}
	MPI_Finalize();
	return 0;
}
