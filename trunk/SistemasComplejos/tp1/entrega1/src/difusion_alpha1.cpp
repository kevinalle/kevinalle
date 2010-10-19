#include<iostream>
#include<cstdlib>
#include<math.h>
using namespace std;

#define forn(i,n) for(int i=0;i<n;i++)
#define forsn(i,s,n) for(int i=(int)(s);i<n;i++)

double F(double x){
	return x<.5?10.:-5.;
}
double LeftBorder(double t){
	return 10.;
}
double RightBorder(double t){
	return -5.;
}

int main(int argc,char**argv){
	double dt=.000025;
	double dx=.01;
	double K=1.; // Coeficiente de difusion termica adimensionalizado
	double L=1.; // Longitud
	double r=dt*K/(dx*dx);
	
	int output_t_samples=20;
	int output_x_samples=100;
	
	double tf=.1;
	
	clog<<"r: "<<r<<endl;
	
	int times=tf/dt+1;
	int samples=L/dx+2;
	double*u=new double[samples];
	double*u2=new double[samples];
	forn(i,samples) u[i]=F((double)i/samples);
	u[0]=LeftBorder(0.);
	u[samples-1]=RightBorder(0.);
	
	forn(i,samples) u2[i]=u[i];
	forn(t,times+1){
		u2[0]=LeftBorder(tf*t/times);
		u2[samples-1]=RightBorder(tf*t/times);
		forn(iter,20){
			forsn(i,1,samples-1){
				u2[i]=(r/(1+2*r))*(u2[i+1]+u2[i-1])+u[i]/(1+2*r);
			}
		}
		double*temp=u; u=u2; u2=temp;
		
		
		if(t%(times/output_t_samples)==0){
			clog << "t=" << t*dt << ":\t";
			for(int i=0;i<samples;i+=samples/output_x_samples) cout << u[i] << " ";
			cout << endl;
		}
	}
	return 0;
}
