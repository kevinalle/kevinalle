#include <iostream>
#include <SDL/SDL.h>
#include <math.h>
#include <vector>
using namespace std;

#define foreach(it,l) for(typeof(l.begin()) it=l.begin();it!=l.end();it++)
#define forn(i,n) for(int i=0;i<(int)(n);i++)

#define W 500
#define H 400
#define OSA 2

typedef struct _color{
	_color(uint _r,uint _g,uint _b):r(_r),g(_g),b(_b){}
	_color(uint c):r(c>>16&0xff),g(c>>8&0xff),b(c&0xff){}
	uint r;
	uint g;
	uint b;
	uint hex(){return r<<16|g<<8|b;}
} Color;

typedef struct _vector{
	_vector():x(0),y(0),z(0){}
	_vector(double _x, double _y, double _z):x(_x),y(_y),z(_z){}
	_vector(double th, double phi):x(cos(th)*sin(phi)),y(sin(th)*sin(phi)),z(cos(phi)){}
	double x;
	double y;
	double z;
	double norm(){return sqrt(x*x+y*y+z*z);}
	_vector normalized(){double n = norm();return _vector(x/n,y/n,z/n);}
	void normalize(){double n = norm();x/=n;y/=n;z/=n;}
	_vector operator +(_vector b){return _vector(x+b.x,y+b.y,z+b.z);}
	_vector operator -(_vector b){return _vector(x-b.x,y-b.y,z-b.z);}
	_vector operator -(){return _vector(-x,-y,-z);}
	double operator *(_vector b){return (x*b.x)+(y*b.y)+(z*b.z);}
	_vector operator *(double n){return _vector(x*n,y*n,z*n);}
	_vector operator ^(_vector v){return _vector(y*v.z-z*v.y ,z*v.x-x*v.z ,x*v.y-y*v.x);}
} Vector;

typedef struct _cam{
	_cam():pos(Vector(0,0,0)),dir(Vector(0,0,1)),up(Vector(0,1,0)),fl(100.),fov(2.),aoSamp(3){}
	_cam(Vector _pos,Vector _dir,Vector _up,double _fl,double _fov,int _aoSamp=5):pos(_pos),dir(_dir.normalized()),up(_up.normalized()),fl(_fl),fov(_fov),aoSamp(_aoSamp){}
	Vector pos;
	Vector dir;
	Vector up;
	double fl;
	double fov;
	int aoSamp;
} Cam;

typedef struct _sphere{
	_sphere(double _r, Vector _c):r(_r),c(_c){}
	double r;
	Vector c;
	
	Vector normal(Vector p){return (p-c).normalized();}
	
	pair<bool,double> intersects(Vector o, Vector d){
		pair<bool,double> res;
		res.first = false;

		// coeficientes de la ecuacion cuadratica
		float A = d*d;
		float B = (o-c)*2*d;
		float C = (o-c)*(o-c)-r*r;

		// calculo el discriminante
		float disc = B*B - 4*A*C;

		// me fijo que sea positivo
		if(disc<0) return res;

		// calculo las soluciones
		float s = sqrt(disc);
		float t0 = (-B - s)/2; 
		float t1 = (-B + s)/2;

		if(t0<0 || t1<0) return res;

		// me aseguro que t0 < t1 para devolver la interseccion mas cercana
		/*if(t1<t0){
			float temp = t0;
			t0 = t1;
			t1 = temp;
		}*/
		t0=min(t0,t1);

		res.first = true;
		res.second = t0;
		return res;
	}
} Sphere;

typedef pair<bool,double> Intersection;

void drawpix(SDL_Surface* surface, int x, int y, Color c){
	Uint32* pixels = (Uint32*)surface->pixels;
	pixels[(y*surface->w)+x] = SDL_MapRGB(surface->format, c.r, c.g, c.b);
}

Intersection ray(Vector from, Vector dir, vector<Sphere>& obj){
	return Intersection(false,0);
}

int render(SDL_Surface* screen){
	Cam cam;
	vector<Sphere> obj;
	obj.push_back(Sphere(3,Vector(0,0,10)));
	
	double alpha=2*cam.fl*tan(cam.fov/2)/W;
	Vector dx=(cam.dir^cam.up).normalized()*alpha;
	Vector dy=cam.up*alpha;
	forn(y,H){
		forn(x,W){
			Vector raydir=cam.dir*cam.fl+dx*(x-W/2.)+dy*(y-H/2.);
			Intersection pI=obj[0].intersects(cam.pos,raydir);
			int c=cam.aoSamp;
			if(pI.first){
				Vector I=raydir*pI.second;
				Vector N=obj[0].normal(I);
				forn(i,cam.aoSamp){
					ray(I,N,obj);
				}
			}
			drawpix(screen, x,y, Color(c,c,c));
		}
		if(y%5==4) if(SDL_Flip(screen)==-1) return -1;
		SDL_Event event; while(SDL_PollEvent(&event)) if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE) return -1;
	}
	return 0;
}

int main(int argc, char* argv[]){
	/* inicio SDL */
	if(SDL_Init(SDL_INIT_EVERYTHING)==-1) return 1;

	SDL_Surface* screen = NULL;
	screen = SDL_SetVideoMode(W, H, 32, SDL_SWSURFACE);
	if(!screen) return 1;
	if(render(screen)==-1) return 1;

	SDL_Event event;
	bool quit = false;
	while(!quit){
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE) quit=true;
		}
	}

	SDL_Quit(); 

	return 0;
}
