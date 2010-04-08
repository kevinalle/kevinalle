#include <iostream>
#include <SDL/SDL.h>
#include <math.h>
#include <vector>

using namespace std;

#define foreach(it,l) for(typeof(l.begin()) it=l.begin();it!=l.end();it++)
#define forn(i,n) for(int i=0;i<(int)(n);i++)

#define W 800
#define H 500
#define OSA 2
#define AOSAMP 20
#define DOFSAMP 3

typedef struct _color{
	_color(int _r,int _g,int _b):r(_r),g(_g),b(_b){}
	_color(int c):r(c>>16&0xff),g(c>>8&0xff),b(c&0xff){}
	int r;
	int g;
	int b;
	int hex(){return r<<16|g<<8|b;}
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
	_cam():pos(Vector(0,0,0)),dir(Vector(0,0,1)),up(Vector(0,1,0)),fl(10.),fov(1.2){}
	_cam(Vector _pos, Vector _lookat):pos(_pos),dir((_lookat-_pos).normalized()),up(Vector(0,1,0)),fl(10.),fov(1.2){}
	_cam(Vector _pos,Vector _dir,Vector _up,double _fl,double _fov):pos(_pos),dir(_dir.normalized()),up(_up.normalized()),fl(_fl),fov(_fov){}
	Vector pos;
	Vector dir;
	Vector up;
	double fl;
	double fov;
} Cam;

typedef struct _sphere{
	_sphere(double _r, Vector _c):r(_r),c(_c){}
	double r;
	Vector c;
	
	Vector normal(Vector p){return (p-c).normalized();}
	
	double intersects(Vector o, Vector d){
		// coeficientes de la ecuacion cuadratica
		double A = d*d;
		double B = (o-c)*2*d;
		double C = (o-c)*(o-c)-r*r;

		// calculo el discriminante
		double disc = B*B - 4*A*C;

		// me fijo que sea positivo
		if(disc<0) return -1;

		// calculo las soluciones
		double s = sqrt(disc);
		double t0 = (-B - s)/2.; 
		double t1 = (-B + s)/2.;

		if(t0<0 || t1<0) return -1;
		return min(t0,t1);
	}
} Sphere;

typedef struct _inter{
	_inter():hit(false),dist(INFINITY){}
	_inter(double _dist, Sphere* _what):hit(true),dist(_dist),what(_what){}
	bool hit;
	double dist;
	Sphere* what;
} Intersection;

void drawpix(SDL_Surface* surface, int x, int y, Color c){
	Uint32* pixels = (Uint32*)surface->pixels;
	pixels[(y*surface->w)+x] = SDL_MapRGB(surface->format, c.r, c.g, c.b);
}

Intersection ray(Vector from, Vector dir, vector<Sphere>& obj){
	Intersection res;
	foreach(it,obj){
		double I=it->intersects(from,dir);
		if(I>=0 && (I<res.dist || !res.hit)){
			res.hit=true;
			res.dist=I;
			res.what=&*it;
		}
	}
	return res;
}

int render(SDL_Surface* screen){
	Cam cam(Vector(0,6,6),Vector(0,-1,20));
	vector<Sphere> obj;
	/*forn(s,15){
		float r=rand()%300/200.+2;
		float x=(rand()%200-100)/5.;
		float z=(rand()%200-100)/5.+40;
		obj.push_back(Sphere(r,Vector(x,r-6,z)));
		cout << "obj.push_back(Sphere("<<r<<",Vector("<<x<<","<<r-6<<","<<z<<")));" << endl;
	}*/
	obj.push_back(Sphere(3.2,Vector(10.6,-2.8,25)));
	obj.push_back(Sphere(2.605,Vector(11,-3.395,33.6)));
	obj.push_back(Sphere(2.615,Vector(1,-3.385,20.2)));
	obj.push_back(Sphere(2.485,Vector(-11,-3.515,40.4)));
	obj.push_back(Sphere(3.25,Vector(-17.6,-2.75,36.8)));
	obj.push_back(Sphere(2.97,Vector(-3.8,-3.03,46.8)));
	obj.push_back(Sphere(3.415,Vector(-2.4,-2.585,40.2)));
	obj.push_back(Sphere(2.28,Vector(8.8,-3.72,59)));
	obj.push_back(Sphere(2.535,Vector(-9.8,-3.465,20)));
	obj.push_back(Sphere(2.72,Vector(-3.4,-3.28,29.6)));
	obj.push_back(Sphere(3.06,Vector(6.2,-2.94,39.8)));
	obj.push_back(Sphere(8000,Vector(0,-8006,30)));
	
	double alpha=2*cam.fl*tan(cam.fov/2)/W;
	Vector dx=(cam.up^cam.dir).normalized()*alpha;
	Vector dy=(dx^cam.dir).normalized()*alpha;
	double beta=alpha*3.5;
	Vector dof_dx=dx.normalized()*beta;
	Vector dof_dy=dy.normalized()*beta;
	forn(y,H){
		forn(x,W){
			int col=0;
			forn(xdof,DOFSAMP) forn(ydof,DOFSAMP){
				forn(xx,OSA) forn(yy,OSA){
					Vector dofpos=dof_dx*xdof+dof_dy*ydof;
					Vector raydir=((cam.dir*cam.fl+dx*(x-W/2.+(float)xx/OSA)+dy*(y-H/2.+(float)yy/OSA))-dofpos).normalized();
					Intersection pI=ray(cam.pos+dofpos,raydir,obj);
					double c=0;
					if(pI.hit){
						Vector I=cam.pos+raydir*pI.dist;
						Vector N=pI.what->normal(I);
						forn(i,AOSAMP){
							Vector dir;
							do dir=Vector(rand()%1000-500,rand()%1000-500,rand()%1000-500).normalized(); while(acos(dir*N)>1.5);
							c+=atan(ray(I,dir,obj).dist/5)/1.6;
						}
						col+=c*255./AOSAMP;
						//col=0;
					}else col+=255;
				}
			}
			col/=OSA*OSA*DOFSAMP*DOFSAMP;
			drawpix(screen, x,y, Color(col,col,col));
		}
		if(y%5==4) if(SDL_Flip(screen)==-1) return -1;
		SDL_Event event; while(SDL_PollEvent(&event)) if(event.type == SDL_QUIT || event.type == SDL_KEYDOWN && event.key.keysym.sym==SDLK_ESCAPE) return -1;
	}
	return 0;
}

int main(int argc, char* argv[]){
	srand(time(NULL));
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
