descripcion="""
<chebot tamanio="7.5" color="0,200,0">
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

	TIMER_ESCAPE_LINEA=70
#	TIMER_ROTANDO=135
	TIMER_SCAN_LOCAL=100
	TEL_LIM=75
	V_SCAN_GLOBAL_1=20
	V_SCAN_GLOBAL_2=19
	V_ESCAPE_LINEA=30
#	V_ROTANDO=30
	V_SCAN_LOCAL=30

	def Escape_linea():
		self.contador_escape_linea+=1*self.clk5ms
		enviarComandoDeVelocidad(V_ESCAPE_LINEA,V_ESCAPE_LINEA)
		if self.contador_escape_linea>=TIMER_ESCAPE_LINEA:
			self.contador_scan_local=0
			self.status="STATUS_SCAN_LOCAL"

	def Scan_global():
		if self.lf.dato==1:
			self.contador_escape_linea=0
			self.status="STATUS_ESCAPE_LINEA"
		elif self.ir0.dato > TEL_LIM or self.ir1.dato > TEL_LIM or self.ir2.dato > TEL_LIM:
			self.status = "STATUS_ATTACK"
		elif self.scan_global_dir==0:
			enviarComandoDeVelocidad(-V_SCAN_GLOBAL_2,-V_SCAN_GLOBAL_1)
		else:
			enviarComandoDeVelocidad(-V_SCAN_GLOBAL_1,-V_SCAN_GLOBAL_2)
			

	def Scan_local():
		self.contador_scan_local+=1*self.clk5ms
		if self.contador_scan_local>=TIMER_SCAN_LOCAL:
			self.status="STATUS_SCAN_GLOBAL"
			self.scan_global_dir=1-self.scan_global_dir
		elif self.ir0.dato > TEL_LIM or self.ir1.dato > TEL_LIM or self.ir2.dato > TEL_LIM:
			self.status = "STATUS_ATTACK"
		elif self.ultima_pos=="DERECHA":
			enviarComandoDeVelocidad(-V_SCAN_LOCAL,V_SCAN_LOCAL)
		else:
			enviarComandoDeVelocidad(V_SCAN_LOCAL,-V_SCAN_LOCAL)

#	def Rotando():
#		self.contador_rotando+=1*5
#		if self.contador_rotando==TIMER_ROTANDO:
#			self.status="STATUS_SCAN_GLOBAL"
#		elif self.ultima_pos=="DERECHA":
#			enviarComandoDeVelocidad(-V_ROTANDO,V_ROTANDO)
#		else:
#			enviarComandoDeVelocidad(V_ROTANDO,-V_ROTANDO)

	def Attack():
		if self.lf.dato==1:
			self.contador_escape_linea=0
			self.status="STATUS_ESCAPE_LINEA"
		elif self.ir1.dato > TEL_LIM:
			self.ultima_pos = "FRENTE"
			enviarComandoDeVelocidad(-30,-30)
		elif self.ir0.dato > TEL_LIM:
			self.ultima_pos = "DERECHA"
			enviarComandoDeVelocidad(-30,0);
		elif self.ir2.dato > TEL_LIM:
			self.ultima_pos = "IZQUIERDA"
			enviarComandoDeVelocidad(0,-30)
		else:
			self.contador_scan_local=0
			self.status = "STATUS_SCAN_LOCAL"

	if self.status=="STATUS_ESCAPE_LINEA": Escape_linea()
	elif self.status=="STATUS_SCAN_GLOBAL": Scan_global()
	elif self.status=="STATUS_SCAN_LOCAL": Scan_local()
#	elif self.status=="STATUS_ROTANDO": Rotando()
	else: Attack()
