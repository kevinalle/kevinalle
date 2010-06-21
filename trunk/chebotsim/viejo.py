descripcion="""
<chebot tamanio="7.5" color="200,200,200">
	<habilidades>
		<LineFollower name="lf" pos="2,0"/>
		<Telemetro name="ir0" pos="5,5" angulo="35" distancia="16"/>
		<Telemetro name="ir2" pos="5,-5" angulo="-35" distancia="16"/>
		<Telemetro name="ir1" pos="5,0" angulo="0" distancia="16"/>
	</habilidades>
</chebot>
"""

def init(self):
	self.status="STATUS_SCAN_GLOBAL"
	self.contador_escape_linea = 0
	self.ultima_pos = "DERECHA"
	self.scan_global_dir = 0

def comportamiento(self):
	enviarComandoDeVelocidad=self.enviarComandoDeVelocidad
	#for name,lfw in self.linefs.items(): exec('%s = lfw'%name)
	#for name,tm in self.teles.items(): vars()[name] = tm
	
	TIMER_SCAN_LOCAL=50
	TIMER_ESCAPE_LINEA=210
	TIMER_MOCHO=100
	TEL_LIM=80
	V_SCAN_LOCAL=20
	V_SCAN_GLOBAL_1=20
	V_SCAN_GLOBAL_2=17
	V_ESCAPE_LINEA=30
	V_MOCHO=40
	
	def Escape_linea():
		enviarComandoDeVelocidad(V_ESCAPE_LINEA,V_ESCAPE_LINEA/2)
		self.contador_escape_linea+=1*self.clk5ms
		if self.contador_escape_linea>=TIMER_ESCAPE_LINEA: self.status = "STATUS_SCAN_GLOBAL"
	
	def Scan_global():
		if self.lf.dato:
			self.contador_escape_linea = 0
			self.status = "STATUS_ESCAPE_LINEA"
		elif self.ir0.dato > TEL_LIM or self.ir1.dato > TEL_LIM or self.ir2.dato > TEL_LIM:
			self.status = "STATUS_ATTACK"
		elif self.scan_global_dir==0:
			enviarComandoDeVelocidad(-V_SCAN_GLOBAL_1,-V_SCAN_GLOBAL_2);
			self.scan_global_dir=1;
		else:
			enviarComandoDeVelocidad(-V_SCAN_GLOBAL_2,-V_SCAN_GLOBAL_1);
			self.scan_global_dir=0;
		
	def Scan_local():
		self.contador_scan_local+=1*self.clk5ms
		if self.contador_scan_local>=TIMER_SCAN_LOCAL: self.status = "STATUS_SCAN_GLOBAL"
		if self.lf.dato:
			self.contador_escape_linea = 0
			self.status = "STATUS_ESCAPE_LINEA"
		elif self.ir0.dato > TEL_LIM or self.ir1.dato > TEL_LIM or self.ir2.dato > TEL_LIM:
			self.status = "STATUS_ATTACK"
		elif self.ultima_pos=="DERECHA":
			enviarComandoDeVelocidad(-V_SCAN_LOCAL,V_SCAN_LOCAL);
		else:
			enviarComandoDeVelocidad(V_SCAN_LOCAL,-V_SCAN_LOCAL);	
		
	def Mocho():
		self.contador_mocho+=1*self.clk5ms
		if self.contador_mocho>=TIMER_MOCHO: self.status = "STATUS_ATTACK"
		enviarComandoDeVelocidad(V_MOCHO,V_MOCHO)

	def Attack():
		if self.lf.dato:
			self.contador_mocho = 0
			self.status = "STATUS_MOCHO"
		elif self.ir1.dato > TEL_LIM:
			self.ultima_pos = "FRENTE"
			enviarComandoDeVelocidad(-30,-30)
		elif self.ir0.dato > TEL_LIM:
			self.ultima_pos = "DERECHA"
			enviarComandoDeVelocidad(-20,0);
		elif self.ir2.dato > TEL_LIM:
			self.ultima_pos = "IZQUIERDA"
			enviarComandoDeVelocidad(0,-20)
		else:
			if self.ir0 <= TEL_LIM and self.ir1 <= TEL_LIM and self.ir2 <= TEL_LIM:
				self.contador_scan_local = 0
				self.status = "STATUS_SCAN_LOCAL"

	if self.status=="STATUS_ESCAPE_LINEA": Escape_linea()
	elif self.status=="STATUS_SCAN_GLOBAL": Scan_global()
	elif self.status=="STATUS_SCAN_LOCAL": Scan_local()
	elif self.status=="STATUS_MOCHO": Mocho()
	else: Attack()
