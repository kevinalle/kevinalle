#include <iostream>
#include <elas.h>
#include "stdio.h"
#include "cv.h"
#include "highgui.h"
// #include "libexabot-remote/libexabot-remote.h"

#define VIDEO1 "../video/cam1.avi"
#define VIDEO2 "../video/cam2.avi"
#define PARAMXML "./parameters.xml"
#define VIDEOOUT "../video/out.avi"
#define SIZE 200
#define CROP 8 // %
#define BUFFER 10
#define THRESH1 4.0
#define THRESH2 8.0
#define RANGO_MOTOR 100

using namespace std;
using namespace cv;


/* Estructuras Auxiliares */
/* ====================== */

struct image_data{
	Mat P1;
	Mat P2;
	Mat dist1;
	Mat dist2;
	Mat R;
	Mat T;
};

struct rect_params {
	Mat undistortion;
	Mat rectification;
};

struct cv_8uc3{
	uchar r;
	uchar g;
	uchar b;
};


/* Declaracion de Funciones Auxiliares */
/* =================================== */

bool cargar_frames(VideoCapture &vid1, Mat &m1, VideoCapture &vid2, Mat &m2);
double promedio(Mat disp, int xmin, int ymin, int xmax, int ymax);
void estrategia(double pizq, double pmed, double pder, double &motor_izq, double &motor_der);
int obtener_parametros_imagenes(image_data &parametros);
void obtener_parametros_rectificacion(const image_data in, rect_params &out1, rect_params &out2, const Size &tam);
void rectificar(const rect_params &info, const Mat &in, Mat &out);
void desplazar(int desp, const Mat &in, Mat &out);
void mezclar_imagenes(const Mat &m1, const Mat &m2, Mat &out);
int obtener_ajuste_horizontal(const Mat &m1, const Mat &m2);
void color (float disp, uchar &r, uchar &g, uchar&b);
void colorear (Mat &disp, Mat &colordisp);
int amin3(double a, double b, double c);


/* Algoritmo Principal */
/* =================== */

int main(int argc, char* argv[]) {

	// Matrices y variables a utilizar
	Mat	fl, 							// Imagen izquierda en grayscale
		fr,								// Imagen derecha en grayscale
		fl_rect,						// Imagen izquierda en grayscale y rectificada
		fr_rect,						// Imagen derecha en grayscale y rectificada
		merge,							// Imagen 3D
		merge_disp,						// Imagen de diparidad
		right,
		right_small,
		left,
		left_small
		;

	image_data matlab_params;			// Estructura con los datos de la calibracion estereo

	rect_params rect_left,				// Estructura con las matrices necesarias para rectificar la imagen izquierda
			  rect_right;				// Estructura con las matrices necesarias para rectificar la imagen derecha


	int dims[3];
	Elas::parameters data;
	Elas elas(data);

	namedWindow("Video 3D");			// Ventana donde se muestra el video 3D
	namedWindow("Video Disparidad");	// Ventana donde se muestra el video del mapa de disparidad
	VideoWriter out;					// Archivo del video de salida

	// Abro los videos
	string video1, video2;
	if (argc > 2) {
    	video1 = argv[1];
    	video2 = argv[2];
    } else {
        video1 = VIDEO1;
        video2 = VIDEO2;
    }
	VideoCapture video_right(video1);
	VideoCapture video_left(video2);
	if(!video_right.isOpened() || !video_left.isOpened()){ cerr << "Error al abrir el video" << endl; return -1; }

	// Cargo un par de frames
	cargar_frames(video_right, fr, video_left, fl);
	
	obtener_parametros_imagenes(matlab_params);

	// Obtengo los parametros necesarios para rectificar ambas imagenes
	obtener_parametros_rectificacion(matlab_params, rect_right, rect_left, fr.size());

	// Rectifico la imagen derecha
	rectificar(rect_right, fr, fr_rect);

	// Rectifico la imagen izquierda
	rectificar(rect_left, fl, fl_rect);

	// Obtengo el numero de pixeles que hay que deplazar la imagen izquierda
	int desp = obtener_ajuste_horizontal(fr_rect, fl_rect);
	
	// Tamanio final
	int newsize = SIZE*(100+CROP)/100;
	float cropp = CROP/100.;
	Size2i size(newsize, fr_rect.rows * newsize / fr_rect.cols);
	Rect crop(size.width*cropp, size.height*cropp, size.width*(1-2*cropp), size.height*(1-2*cropp));

	// Otras matrices a utilizar
	Mat fl_rect2(fl.size(), fl.type());  // Matriz izquierda con el corrimiento aplicado
	Mat mapDis1(size, CV_32F);           // Mapa de disparidad de la imagen derecha
	Mat mapDis2(size, CV_32F);           // Mapa de disparidad de la imagen izquierda
	Mat colorDisp(size, CV_8UC3);        // Mapa de disparidad con colores
	dims[0] = size.width;		         // Inicializo
	dims[1] = size.height;
	dims[2] = size.width;

	// Abro el archivo de salida 'out.avi'
	// out.open(VIDEOOUT,CV_FOURCC('D','I','V','X'), 30, colorDisp.size(), 1);
	
	// Comunicacion Exabot
	// exa_remote_initialize("192.168.1.2");

	// Proceso los restantes frames del video
	unsigned int frameno = 0;
	double pizq = 0;
	double pmed = 0;
	double pder = 0;
	double motor_izq = 0.;
	double motor_der = 0.;
	while(cargar_frames(video_right, fr, video_left, fl)) {
		// Obtengo el frame en 3D
		rectificar(rect_right, fr, fr_rect);
		rectificar(rect_left, fl, fl_rect);
		desplazar(desp, fl_rect, fl_rect2);
		
		// Crop
		resize(fr_rect, right_small, size);
		right = right_small(crop);
		resize(fl_rect2, left_small, size);
		left = left_small(crop);
		mezclar_imagenes(left, right, merge);

		// Obtengo el mapa de disparidad
		elas.process(left.data, right.data, (float*)mapDis1.data, (float*)mapDis2.data, dims);
		colorear(mapDis1, colorDisp);
		
		// Calculo direccion
	    pizq += promedio(mapDis1, 1, 1, size.width/3, size.height);
	    pmed += promedio(mapDis1, size.width/3, 1, 2*size.width/3, size.height);
	    pder += promedio(mapDis1, 2*size.width/3, 1, size.width, size.height);
		if (frameno % BUFFER == BUFFER-1) {
		    pizq /= BUFFER; pmed /= BUFFER; pder /= BUFFER;
		    estrategia(pizq, pmed, pder, motor_izq, motor_der);
		    int motorI = (int)(motor_izq*RANGO_MOTOR);
		    int motorD = (int)(motor_der*RANGO_MOTOR);
	        cout << "[" << motorI << ", " << motorD << "]" << endl;
	        // exa_remote_set_motors(motorI, motorD);
		    pizq = 0; pmed = 0; pder = 0;
		}

		// Muestro ambos frames en las ventanas
		imshow("Video 3D", merge);
		imshow("Video Disparidad", colorDisp);

		// Guardo el frame con la imagen de disparidad en el archivo 'out.avi'
		// out << colorDisp;

		// FPS = 30
		if (waitKey(30) >= 0)
			break;

		frameno++;
	}
	// exa_remote_deinitialize();
	return 0;
}


/* Implementacion de Funciones Auxiliares */
/* ====================================== */

bool cargar_frames(VideoCapture &vid1, Mat &m1, VideoCapture &vid2, Mat &m2) {
	Mat tmp1, tmp2;
	if (vid1.grab() && vid2.grab()) {
    	vid1.retrieve(tmp1);
    	vid2.retrieve(tmp2);
	    cvtColor(tmp1, m1, CV_BGR2GRAY);
	    cvtColor(tmp2, m2, CV_BGR2GRAY);
	    return true;
	} else {
	    return false;
	}
}

double promedio(Mat disp, int xmin, int ymin, int xmax, int ymax) {
    double sum = 0.;
    unsigned int total = 1;
    for (int j=ymin; j<ymax; j++) for (int i=xmin; i<xmax; i++) if (disp.at<float>(j, i)>0) {
        sum += disp.at<float>(j, i);
        total++;
    }
    return sum/total; //((xmax-xmin)*(ymax-ymin));
}

void estrategia(double pizq, double pmed, double pder, double &motor_izq, double &motor_der) {
    if (pmed < THRESH1) {
        motor_izq = 0.5 + 0.4*(pder<THRESH2);
        motor_der = 0.5 + 0.4*(pizq<THRESH2);
        return;
    }
    if (pmed < THRESH2) {
        motor_izq = 0.1 + 0.4*(pder<THRESH1);
        motor_der = 0.1 + 0.4*(pizq<THRESH1);
        return;
    }
    if (pizq > THRESH2 && pder > THRESH2) {
        motor_izq = 0.4;
        motor_der = -0.5;
        return;
    }
    if (pizq < pder) {
        motor_izq = -0.2;
        motor_der = 0.4;
    } else {
        motor_izq = 0.4;
        motor_der = -0.2;
    }
}

int obtener_parametros_imagenes(image_data &parametros) {
	FileStorage file = FileStorage(PARAMXML, FileStorage::READ);
	if (!file.isOpened())
		return -1;
	FileNode rootnode = file.root();
	rootnode["P1"] 		>> parametros.P1;
	rootnode["dist1"] 	>> parametros.dist1;
	rootnode["P2"]		>> parametros.P2;
	rootnode["dist2"] 	>> parametros.dist2;
	rootnode["R"]		>> parametros.R;
	rootnode["T"]		>> parametros.T;
	file.release();
	return 0;
}

void obtener_parametros_rectificacion (const image_data in, rect_params &out1, rect_params &out2, const Size &tam)  {
	Mat P1_rect, P2_rect, R1, R2, Q;
	stereoRectify(in.P1, in.dist1, in.P2, in.dist2, tam, in.R, in.T, R1, R2, P1_rect, P2_rect, Q, CALIB_ZERO_DISPARITY);
	initUndistortRectifyMap(in.P1, in.dist1, R1, P1_rect, tam, CV_32FC1, out1.undistortion, out1.rectification);
	initUndistortRectifyMap(in.P2, in.dist2, R2, P2_rect, tam, CV_32FC1, out2.undistortion, out2.rectification);
}

void rectificar(const rect_params &info, const Mat &in, Mat &out) {
	remap(in, out, info.undistortion, info.rectification, CV_INTER_LINEAR||CV_WARP_FILL_OUTLIERS);
}

int obtener_ajuste_horizontal (const Mat &m1, const Mat &m2) {
	int medio = 128;
	int delta_x = medio;
	Mat mezcla;
	Mat desplazada(m1.size(), m1.type());
	namedWindow("Alineación Horizontal", CV_WINDOW_AUTOSIZE);
	createTrackbar("ajuste","Alineación Horizontal",&delta_x,255,NULL);

	while(1){
		desplazar(delta_x-medio, m2, desplazada);
		mezclar_imagenes(desplazada, m1, mezcla);
		imshow("Alineación Horizontal", mezcla);
		if (waitKey(30) >= 0)
			break;
	}
	cvDestroyWindow("Alineación Horizontal");
	return delta_x-medio;
}

void desplazar (int desp, const Mat &in, Mat &out) {
	int pos = 0;
	for(int j=0; j<out.cols; j++){
		pos=j-desp;
		for(int i=0; i<out.rows; i++) {
			if (pos>=0 && pos<out.cols)
				out.at<uchar>(i,j) = in.at<uchar>(i,pos);
			else
				out.at<uchar>(i,j) = 0.;
		}
	}
}

void mezclar_imagenes (const Mat &m1, const Mat &m2, Mat &out) {
	double alpha = 0.5;
	addWeighted(m2,alpha,m1,alpha,0,out);
}

void color (float disp, uchar &r, uchar &g, uchar&b) {
	float val = min(disp*0.01f,1.0f);
	/*int gray = (int)(val*255);
	r = gray;
	g = gray;
	b = gray;*/
	if (val <= 0) {
		r = 0;
		g = 0;
		b = 0;
	} else {
		float h2 = 6.0f * (1.0f - val);
		unsigned char x  = (unsigned char)((1.0f - fabs(fmod(h2, 2.0f) - 1.0f))*255);
		if (0 <= h2 && h2<1) {
			r = 255;
			g = x;
			b = 0;
		} else if (1<=h2 && h2<2) {
			r = x;
			g = 255;
			b = 0;
		} else if (2<=h2 && h2<3) {
			r = 0;
			g = 255;
			b = x;
		} else if (3<=h2 && h2<4) {
			r = 0;
			g = x;
			b = 255;
		} else if (4<=h2 && h2<5) {
			r = x;
			g = 0;
			b = 255;
		} else if (5<=h2 && h2<=6) {
			r = 255;
			g = 0;
			b = x;
		}
	}
}

void colorear (Mat &disp, Mat &colordisp) {
	cv_8uc3 rgb;
	for(int j=0; j<colordisp.cols; j++) for(int i=0; i<colordisp.rows; i++){
		color(disp.at<float>(i,j),rgb.r,rgb.g,rgb.b);
		((cv_8uc3*)colordisp.data)[i*colordisp.cols+j] = rgb;
	}
}

int amin3(double a, double b, double c) {
    if (a<b && a<c) return 0;
    if (b<c) return 1;
    return 2;
}
