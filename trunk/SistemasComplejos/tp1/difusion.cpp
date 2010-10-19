#include<iostream>
#include<cstdlib>
#include<math.h>
using namespace std;

#define forn(i,n) for(int i=0;i<n;i++)
#define forsn(i,s,n) for(int i=(int)(s);i<n;i++)

//Condiciones Iniciales y de Borde
double F(double x){return 0;}
double LeftBorder(double t){return 10.;}
double RightBorder(double t){return -5.;}

int main(int argc,char**argv){
	double dt=.000025;
	double dx=.01;
	double K=1.; // Coeficiente de difusion termica adimensionalizado
	double L=1.; // Longitud
	double r=dt*K/(dx*dx);
	
	int output_t_samples=30; //cantidad de lineas de salida
	int output_x_samples=100; //resolucion del vector de salida
	
	double tf=.1;	//tiempo final
	int times=tf/dt+1; //cantidad de intervalos de tiempo
	int samples=L/dx+2; //cantidad de muestras espaciales
	double*u=new double[samples];
	double*u2=new double[samples];
	forn(i,samples) u[i]=F(i/samples);
	u[0]=LeftBorder(0.);
	u[samples-1]=RightBorder(0.);
	
	forn(t,times+1){
		//Calculamos los nuevos datos en u2 usando u
		u2[0]=LeftBorder(tf*t/times);
		u2[samples-1]=RightBorder(tf*t/times);
		forsn(i,1,samples-1){
			u2[i]=r*u[i+1]+(1-2*r)*u[i]+r*u[i-1];
		}
		//intercambiamos u con u2
		double*temp=u; u=u2; u2=temp;
		
		//exportar el resultado cada algunas iteraciones 
		if(t%(times/output_t_samples)==0){
			clog << "t=" << t*dt << ":\t";
			for(int i=0;i<samples;i+=samples/output_x_samples) cout << u[i] << " ";
			cout << endl;
		}
	}
	return 0;
}
