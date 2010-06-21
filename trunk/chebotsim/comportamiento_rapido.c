#include <p18f452.h>
#include "ipc.h"
#include "i2c.h"
#include "comandos.h"
#include <delays.h>
#include "ir.h"
#include "timer.h"
#include "timer2.h"
#include "lf.h"

#define TEL_LIM 80

#define TIMER_GIRO_INICIAL 100
#define TIMER_ESCAPE_LINEA 40
#define TIMER_GIRO_ESCAPE 70
#define TIMER_SCAN_LOCAL 70

#define SENSOR_IZQ 2
#define SENSOR_CEN 0
#define SENSOR_DER 1

#define VELOCIDAD_GIRO_INICIAL 25
#define VELOCIDAD_ESCAPE_LINEA 70
#define VELOCIDAD_GIRO_ESCAPE 20
#define VELOCIDAD_SCAN_GLOBAL_IZQ -39
#define VELOCIDAD_SCAN_GLOBAL_DER -40
#define VELOCIDAD_ATTACK 20
#define VELOCIDAD_GIRO_ATTACK 15
#define VELOCIDAD_SCAN_LOCAL_CORTA 0
#define VELOCIDAD_SCAN_LOCAL_LARGA 25

enum{
	STATUS_SCAN_GLOBAL,
	STATUS_SCAN_LOCAL,
	STATUS_ATTACK,
	STATUS_ESCAPE_LINEA,
	STATUS_GIRO_INICIAL,
	STATUS_GIRO_ESCAPE
};

enum{
	IZQ,
	CEN,
	DER
};

#pragma udata access accessram

	near char estadoGlobalNuevoComando;

	/* Guarda las ultimas mediciones validas de los telemetros 0 1 y 2 respectivamente */
	near unsigned char ir_res[3];
	near unsigned char ir_res_ant[3];

	/* Guardo los ultimos resultados validos del LF */
	near unsigned char lf_res;

	/* Flag de clock cada 5ms */
	near unsigned char clk5;

	/* Estado mental del botito */
	near unsigned char status;

	/* contadores */
	near unsigned short contador_5s;
	near unsigned char contador_escape_linea;
	near unsigned char contador_girando;
	near unsigned char contador_scan_local;

	/* Flag de nueva medicion */
	near unsigned char nueva_medicion;

	/* contador para el filtro */
	near unsigned char contador_nueva_medicion;

	/* ultima posicion donde lo vio */

	near unsigned char ultima_pos;

#pragma udata
#pragma code mainProg

void INTInicializar(void);

void main(void){

	// 1) INICIALIZACION DE MODULOS
	i2cInicializarModulo();	// inicializar comunicaciones I2C
	INTInicializar();		// inicializar y habilitar interrupciones
	INTCONbits.GIEH = 1;	// configurar interrupciones de prioridad alta
	INTCONbits.GIEL = 1;	// configurar interrupciones de prioridad baja
	ir_init();				// inicializar el modulo IR
	lf_init();				// inicializar el modulo LF
	timer_init();			// inicializar el timer0 (IR)
	timer2_init();			// inicializar el timer2 (LF)
	
	// 2) NO TOCAR: esto hay que dejarlo así!
	enviarComandoApagarRS232();			// apagar RS232 de los pics de los motores
	enviarComandoApagarRecuperacion();	// apagar recuperacion de los encoders en los motores.

	// 3) APAGAR LEDS: lo usamos para Debug...
	enviarComandoApagarLeds();

	clk5 = 0;
	contador_5s = 0;
	contador_girando = 0;
	contador_escape_linea = 0;
	contador_scan_local = 0;
	nueva_medicion = 0;
	contador_nueva_medicion=0;
	ultima_pos = CEN;
	status = STATUS_GIRO_INICIAL;	// PARA QUE EMPIECE GIRANDO
	//status = STATUS_SCAN_GLOBAL;

	timer2_start();			// prendo el timer de 5ms
	ir_start();				// enciendo los triggers de los ir y arranca a medir el timer0
	lf_start();				// enciendo el trigger del lf

	//----------------------------------------
	// ACLARACION : HAY QUE SETEAR EL ESTADO SEGUN EL COMPORTAMIENTO INICIAL (status = STATUS_GIRO_INICIAL)
	//----------------------------------------

	// ----------------------------------------------------------- //
	// ------------- ESPERO 5 SEGUNDOS PARA ARRANCAR ------------- //
	// ----------------------------------------------------------- //

	while(contador_5s < 750){
		if(clk5){
			contador_5s++;
			clk5=0;
		}
	}

	// ----------------------------------------------------------- //
	// ------------- GIRO HASTA ENCONTRAR AL OPONENTE ------------ //
	// ----------------------------------------------------------- //

	while(status==STATUS_GIRO_INICIAL){
		enviarComandoDeVelocidad(VELOCIDAD_GIRO_INICIAL,-VELOCIDAD_GIRO_INICIAL);
		if(clk5){		
			contador_girando++;
			if( (ir_res[SENSOR_DER] > TEL_LIM && ir_res[SENSOR_CEN] > TEL_LIM) || (ir_res[SENSOR_IZQ] > TEL_LIM && ir_res[SENSOR_CEN] > TEL_LIM)){
				status = STATUS_ATTACK;
			}
			clk5=0;
		}
		
		if(contador_girando==TIMER_GIRO_INICIAL){
			contador_girando = 0;
			status = STATUS_SCAN_GLOBAL;
		}
	}

	// ----------------------------------------------------------- //
	// ----------------- COMPORTAMIENTO GENERAL ------------------ //
	// ----------------------------------------------------------- //

	while(1){

	// Analisis del estado del robot
	
		if(clk5){
			lf_int_handler();
			
			if(status==STATUS_ESCAPE_LINEA){
				contador_escape_linea++;
				if(contador_escape_linea==TIMER_ESCAPE_LINEA){
					contador_girando = 0;
					status=STATUS_GIRO_ESCAPE;
				}
			}
		
			else if(status==STATUS_GIRO_ESCAPE){
				contador_girando++;
				if(contador_girando==TIMER_GIRO_ESCAPE){
					status = STATUS_SCAN_GLOBAL;
				}
			}

			else if(status==STATUS_SCAN_LOCAL){
				contador_scan_local++;
				if(contador_scan_local==TIMER_SCAN_LOCAL){
					status = STATUS_SCAN_GLOBAL;
				}
			}

			clk5 = 0;
		}
	

	// Respuesta del robot segun estado
	
		/* ESCAPE LINEA */		
		if(status==STATUS_ESCAPE_LINEA){
			enviarComandoDeVelocidad(VELOCIDAD_ESCAPE_LINEA,VELOCIDAD_ESCAPE_LINEA);
		}

		/* GIRANDO*/
		else if(status==STATUS_GIRO_ESCAPE){
			//if(lf_res==1){
			//	contador_escape_linea = 0;
			//	status = STATUS_ESCAPE_LINEA;
			//}
			if( (ir_res[SENSOR_DER] > TEL_LIM && ir_res[SENSOR_CEN] > TEL_LIM) || (ir_res[SENSOR_IZQ] > TEL_LIM && ir_res[SENSOR_CEN] > TEL_LIM)){
				status = STATUS_ATTACK;
			}else{
				if(ultima_pos==IZQ){
					enviarComandoDeVelocidad(-VELOCIDAD_GIRO_ESCAPE,VELOCIDAD_GIRO_ESCAPE);
				}else{
					enviarComandoDeVelocidad(VELOCIDAD_GIRO_ESCAPE,-VELOCIDAD_GIRO_ESCAPE);
				}
			}

		}
		
		/* AVANZO */
		else if(status==STATUS_SCAN_GLOBAL){
			if(lf_res==1){
				contador_escape_linea = 0;
				status = STATUS_ESCAPE_LINEA;
			}
			else if(
				(ir_res[SENSOR_DER] > TEL_LIM && ir_res_ant[SENSOR_DER] > TEL_LIM) || 
				(ir_res[SENSOR_IZQ] > TEL_LIM && ir_res_ant[SENSOR_IZQ] > TEL_LIM) || 
				(ir_res[SENSOR_CEN] > TEL_LIM && ir_res_ant[SENSOR_CEN] > TEL_LIM)
			){
				status = STATUS_ATTACK;
			}
			enviarComandoDeVelocidad(VELOCIDAD_SCAN_GLOBAL_IZQ,VELOCIDAD_SCAN_GLOBAL_DER);
		}

		else if(status==STATUS_SCAN_LOCAL){
			if(
				(ir_res[SENSOR_DER] > TEL_LIM && ir_res_ant[SENSOR_DER] > TEL_LIM) || 
				(ir_res[SENSOR_IZQ] > TEL_LIM && ir_res_ant[SENSOR_IZQ] > TEL_LIM) || 
				(ir_res[SENSOR_CEN] > TEL_LIM && ir_res_ant[SENSOR_CEN] > TEL_LIM)
			){
				status = STATUS_ATTACK;
			}else if(ultima_pos==IZQ){
				enviarComandoDeVelocidad(-VELOCIDAD_SCAN_LOCAL_LARGA,-VELOCIDAD_SCAN_LOCAL_CORTA);
			}else if(ultima_pos==DER){
				enviarComandoDeVelocidad(-VELOCIDAD_SCAN_LOCAL_CORTA,-VELOCIDAD_SCAN_LOCAL_LARGA);
			}else{
				enviarComandoDeVelocidad(-VELOCIDAD_SCAN_LOCAL_LARGA,-VELOCIDAD_SCAN_LOCAL_LARGA);
			}
		}

		/* ATACO */
		else if(status==STATUS_ATTACK) {
			if(lf_res==1){
				contador_escape_linea = 0;
				status = STATUS_ESCAPE_LINEA;
			}
			if( ir_res[SENSOR_CEN] > TEL_LIM ){
				ultima_pos = CEN;
				enviarComandoDeVelocidad(-VELOCIDAD_ATTACK,-VELOCIDAD_ATTACK);
			}else if(ir_res[SENSOR_IZQ] > TEL_LIM){
				ultima_pos = IZQ;
				enviarComandoDeVelocidad(-VELOCIDAD_GIRO_ATTACK,VELOCIDAD_GIRO_ATTACK);
			}else if(ir_res[SENSOR_DER] > TEL_LIM){
				ultima_pos = DER;
				enviarComandoDeVelocidad(VELOCIDAD_GIRO_ATTACK,-VELOCIDAD_GIRO_ATTACK);
			}else if(
					(ir_res[SENSOR_DER] <= TEL_LIM && ir_res_ant[SENSOR_DER] <= TEL_LIM) && 
					(ir_res[SENSOR_IZQ] <= TEL_LIM && ir_res_ant[SENSOR_IZQ] <= TEL_LIM) && 
					(ir_res[SENSOR_CEN] <= TEL_LIM && ir_res_ant[SENSOR_CEN] <= TEL_LIM)
				){
				contador_scan_local = 0;
				status = STATUS_SCAN_LOCAL;
			}
		}
		
	} // Fin While
} // Fin Main
