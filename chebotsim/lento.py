descripcion="""
<chebot tamanio="7.5" color="0,200,0">
	<habilidades>
		<LineFollower name="lf" pos="4.25,0"/>
		<Telemetro name="irIZQ" pos="5,5" angulo="35" distancia="25"/>
		<Telemetro name="irDER" pos="5,-5" angulo="-35" distancia="25"/>
		<Telemetro name="irCEN" pos="5,0" angulo="0" distancia="25"/>
	</habilidades>
</chebot>
"""

def init(self):
	self.clk5 = 0
	self.contador_5s = 0
	self.contador_girando = 0
	self.contador_escape_linea = 0
	self.contador_scan_local = 0
	self.nueva_medicion = 0
	self.contador_nueva_medicion=0
	self.ultima_pos = "CEN"
	self.status = "STATUS_GIRO_INICIAL"

def comportamiento(self):

	enviarComandoDeVelocidad=self.enviarComandoDeVelocidad
	#for name,lfw in self.linefs.items(): exec('%s = lfw'%name)
	#for name,tm in self.teles.items(): vars()[name] = tm
	ir_res_ant=ir_res=(self.irCEN.dato,self.irDER.dato,self.irIZQ.dato)

	TEL_LIM=80

	TIMER_GIRO_INICIAL=120
	TIMER_ESCAPE_LINEA=40
	TIMER_GIRO_ESCAPE=35
	TIMER_SCAN_LOCAL=70
	
	SENSOR_IZQ=2
	SENSOR_CEN=0
	SENSOR_DER=1

	VELOCIDAD_GIRO_INICIAL=20
	VELOCIDAD_ESCAPE_LINEA=70
	VELOCIDAD_GIRO_ESCAPE=40
	VELOCIDAD_SCAN_GLOBAL_IZQ=-29
	VELOCIDAD_SCAN_GLOBAL_DER=-30
	VELOCIDAD_ATTACK=20
	VELOCIDAD_GIRO_ATTACK=15
	VELOCIDAD_SCAN_LOCAL_CORTA=0
	VELOCIDAD_SCAN_LOCAL_LARGA=25
	
	
	
	if self.contador_girando>=TIMER_GIRO_INICIAL:
		self.contador_girando = 0
		self.status = "STATUS_SCAN_GLOBAL"
		
			
	if self.status=="STATUS_ESCAPE_LINEA":
		self.contador_escape_linea+=1*self.clk5ms
		if self.contador_escape_linea>=TIMER_ESCAPE_LINEA:
			self.contador_girando = 0
			self.status="STATUS_GIRO_ESCAPE"

	elif self.status=="STATUS_GIRO_ESCAPE":
		self.contador_girando+=1*self.clk5ms
		if self.contador_girando>=TIMER_GIRO_ESCAPE:
			self.status = "STATUS_SCAN_GLOBAL"

	elif self.status=="STATUS_SCAN_LOCAL":
		self.contador_scan_local+=1*self.clk5ms
		if self.contador_scan_local>=TIMER_SCAN_LOCAL:
			self.status = "STATUS_SCAN_GLOBAL"
	
	if self.status=="STATUS_ESCAPE_LINEA":
		enviarComandoDeVelocidad(VELOCIDAD_ESCAPE_LINEA,VELOCIDAD_ESCAPE_LINEA)


	#GIRO INICIAL
	if self.status=="STATUS_GIRO_INICIAL":
		enviarComandoDeVelocidad(VELOCIDAD_GIRO_INICIAL,-VELOCIDAD_GIRO_INICIAL)
		self.contador_girando+=1*self.clk5ms
		if (ir_res[SENSOR_DER] > TEL_LIM and ir_res[SENSOR_CEN] > TEL_LIM) or (ir_res[SENSOR_IZQ] > TEL_LIM and ir_res[SENSOR_CEN] > TEL_LIM):
			self.status = "STATUS_ATTACK"
	
	# GIRANDO
	elif self.status=="STATUS_GIRO_ESCAPE":
		#if self.lf.dato==1:
		#self.contador_escape_linea = 0
		#self.status = STATUS_ESCAPE_LINEA
		if (ir_res[SENSOR_DER] > TEL_LIM and ir_res[SENSOR_CEN] > TEL_LIM) or (ir_res[SENSOR_IZQ] > TEL_LIM and ir_res[SENSOR_CEN] > TEL_LIM):
			self.status = "STATUS_ATTACK"
		else:
			if self.ultima_pos=="IZQ":
				enviarComandoDeVelocidad(-VELOCIDAD_GIRO_ESCAPE,VELOCIDAD_GIRO_ESCAPE)
			else:
				enviarComandoDeVelocidad(VELOCIDAD_GIRO_ESCAPE,-VELOCIDAD_GIRO_ESCAPE)

	# AVANZO
	elif self.status=="STATUS_SCAN_GLOBAL":
		if self.lf.dato:
			self.contador_escape_linea = 0
			self.status = "STATUS_ESCAPE_LINEA"
		elif\
			(ir_res[SENSOR_DER] > TEL_LIM and ir_res_ant[SENSOR_DER] > TEL_LIM) or\
			(ir_res[SENSOR_IZQ] > TEL_LIM and ir_res_ant[SENSOR_IZQ] > TEL_LIM) or\
			(ir_res[SENSOR_CEN] > TEL_LIM and ir_res_ant[SENSOR_CEN] > TEL_LIM):
			self.status = "STATUS_ATTACK"
		enviarComandoDeVelocidad(VELOCIDAD_SCAN_GLOBAL_IZQ,VELOCIDAD_SCAN_GLOBAL_DER)

	elif self.status=="STATUS_SCAN_LOCAL":
		if\
			(ir_res[SENSOR_DER] > TEL_LIM and ir_res_ant[SENSOR_DER] > TEL_LIM) or\
			(ir_res[SENSOR_IZQ] > TEL_LIM and ir_res_ant[SENSOR_IZQ] > TEL_LIM) or\
			(ir_res[SENSOR_CEN] > TEL_LIM and ir_res_ant[SENSOR_CEN] > TEL_LIM):
			self.status = "STATUS_ATTACK"
		elif self.ultima_pos=="IZQ":
			enviarComandoDeVelocidad(-VELOCIDAD_SCAN_LOCAL_LARGA,-VELOCIDAD_SCAN_LOCAL_CORTA)
		elif self.ultima_pos=="DER":
			enviarComandoDeVelocidad(-VELOCIDAD_SCAN_LOCAL_CORTA,-VELOCIDAD_SCAN_LOCAL_LARGA)
		else:
			enviarComandoDeVelocidad(-VELOCIDAD_SCAN_LOCAL_LARGA,-VELOCIDAD_SCAN_LOCAL_LARGA)

	# ATACO
	elif self.status=="STATUS_ATTACK":
		if self.lf.dato:
			self.contador_escape_linea = 0
			self.status = "STATUS_ESCAPE_LINEA"
		if ir_res[SENSOR_CEN] > TEL_LIM:
			self.ultima_pos = "CEN"
			enviarComandoDeVelocidad(-VELOCIDAD_ATTACK,-VELOCIDAD_ATTACK)
		elif ir_res[SENSOR_IZQ] > TEL_LIM:
			self.ultima_pos = "IZQ"
			enviarComandoDeVelocidad(-VELOCIDAD_GIRO_ATTACK,VELOCIDAD_GIRO_ATTACK)
		elif ir_res[SENSOR_DER] > TEL_LIM:
			self.ultima_pos = "DER"
			enviarComandoDeVelocidad(VELOCIDAD_GIRO_ATTACK,-VELOCIDAD_GIRO_ATTACK)
		elif\
			(ir_res[SENSOR_DER] <= TEL_LIM and ir_res_ant[SENSOR_DER] <= TEL_LIM) and\
			(ir_res[SENSOR_IZQ] <= TEL_LIM and ir_res_ant[SENSOR_IZQ] <= TEL_LIM) and\
			(ir_res[SENSOR_CEN] <= TEL_LIM and ir_res_ant[SENSOR_CEN] <= TEL_LIM):
			self.contador_scan_local = 0
			self.status = "STATUS_SCAN_LOCAL"
