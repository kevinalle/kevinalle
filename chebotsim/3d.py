#!/usr/bin/env python
from OpenGL.GL import *
from OpenGL.GLU import *
import pygame
from pygame.locals import *
from pygame.time import *
import os, sys
from math import *
import GL
from xml.dom import minidom
from random import randint,random
from pygame.draw import circle as drawCirc, rect as drawRect, polygon as drawPoly, aaline as drawALine, line as drawLine

class Cube(object):
    def __init__(self, size, position=(0,0,0), color=(1.,1.,1.,1.), rot=0): 
        self.position = position
        self.color = color
        self.sze=size
        self.rot=rot
        
    num_faces = 10
    vertices = [ (-.5, -.5, .5),
                 (.5, -.5, .5),
                 (.5, .5, .5),
                 (-.5, .5, .5),
                 (-.5, -.5, -.5),
                 (.5, -.5, -.5),
                 (.5, .5, -.5),
                 (-.5, .5, -.5),
                 (.8,-.5,-.5), #8
                 (.8, -.5,.5), #9
                 (.5,-.2,-.5), #10
                 (.5,-.2,.5),  #11
                 (.8,-.45,-.5), #12
                 (.8, -.45,.5)] #13
    normals = [ (0.0, 0.0, +1.0),  # front
                (0.0, 0.0, -1.0),  # back
                (+1.0, 0.0, 0.0),  # right
                (-1.0, 0.0, 0.0),  # left 
                (0.0, +1.0, 0.0),  # top
                (0.0, -1.0, 0.0),  # bottom
                (sqrt(2),sqrt(2),0), #ang
                (1,0,0),
                (0,0,-1),
                (0,0,1)]
    vertex_indices = [ (0, 1, 2, 3),  # front
                       (4, 5, 6, 7),  # back
                       (1, 5, 6, 2),  # right
                       (0, 4, 7, 3),  # left
                       (3, 2, 6, 7),  # top
                       (0, 1, 5, 4),  # bottom
                       (10,11,13,12),
                       (12,13,9,8),
                       (8,12,10,5),
                       (9,13,11,1)
                       ]

    def render(self):
    
        # Adjust all the vertices so that the cube is at self.position
        vertices = self.vertices #[tuple(Vector3(v) + self.position) for v in self.vertices]
        
        # Draw all 6 faces of the cube
        glPushMatrix()
        glColor3f( *self.color[:3] )
        #while self.rot<0: self.rot+=2*pi
        glTranslatef(*self.position)
        glScalef(self.sze,self.sze,self.sze)
        glRotatef(-degrees(self.rot),0,1,0)
        
        glBegin(GL_QUADS)
        for face_no in xrange(self.num_faces):
            glNormal3dv( self.normals[face_no] )
            v1, v2, v3, v4 = self.vertex_indices[face_no]
            glVertex( vertices[v1] )
            glVertex( vertices[v2] )
            glVertex( vertices[v3] )
            glVertex( vertices[v4] )
        glEnd()
        glColor3f( .8,.8,.8 )
        glPushMatrix()
        glTranslatef(0,0,.5)
        gluCylinder(gluNewQuadric(),.4,.4,.15,20,2)
        glTranslatef(0,0,.02)
        glColor3f( .2,.2,.2 )
        gluCylinder(gluNewQuadric(),.401,.401,.11,20,2)
        glTranslatef(0,0,-.02)
        glColor3f( .8,.8,.8 )
        glTranslatef(0,0,.15)
        gluDisk(gluNewQuadric(),0,.4,20,1)
        glPopMatrix()
        glTranslatef(0,0,-.65)
        gluCylinder(gluNewQuadric(),.4,.4,.15,20,2)
        glTranslatef(0,0,.02)
        glColor3f( .2,.2,.2 )
        gluCylinder(gluNewQuadric(),.401,.401,.11,20,2)
        glTranslatef(0,0,-.02)
        glColor3f( .8,.8,.8 )
        glRotatef(180,1,0,0)
        gluDisk(gluNewQuadric(),0,.4,20,1)
        glPopMatrix()

class Cancha:
	def render(self):
		glPushMatrix()
		glColor3f(.9,.9,.9)
		glRotatef(-90,1,0,0)
		gluDisk(gluNewQuadric(),36,38.5,50,1)
		glColor3f(.2,.2,.2)
		gluDisk(gluNewQuadric(),0,36,50,1)
		#glColor3f(.7,.7,1)
		#gluDisk(gluNewQuadric(),38.5,3600,50,1)
		glPopMatrix()

#Center the Screen
if sys.platform == 'win32' or sys.platform == 'win64': os.environ['SDL_VIDEO_CENTERED'] = '1'

#Initialize PyGame
pygame.init()

#Make a PyGame Window
#   Screen Size
Screen = (640,480) 
icon = pygame.Surface((1,1)); icon.set_alpha(0); pygame.display.set_icon(icon)
pygame.display.set_caption(":)")  
Surface = pygame.display.set_mode(Screen,OPENGL|DOUBLEBUF)
GL.resize(Screen)
GL.init()












FPS=30
#pygame.init()
W,H = 640,480
#screen = pygame.display.set_mode((W, H))
#pygame.display.set_caption('chebot sumo')
#pygame.mouse.set_visible(False)
clk=Clock()
run=True
clk5ms=200./FPS

def i(x,y):
	return (int(x),int(y))

def rotat(p,c,ang):
	x,y=p
	cx,cy=c
	return (cos(ang)*(x-cx)-sin(ang)*(y-cy)+cx,sin(ang)*(x-cx)+cos(ang)*(y-cy)+cy)

def dist(p1,p2):
	x1,y1=p1
	x2,y2=p2
	return sqrt((x1-x2)**2+(y1-y2)**2)

def atan2(x,y):
	if x!=0: at=atan(float(y)/x)
	if x > 0:
		if y >= 0: return at
		else: return at+2*pi
	elif x==0:
		if y>0: return pi/2
		elif y==0: return 0
		else: return pi*1.5
	else: return at+pi


def rotrect(screen,pos,sze,rot=0):
	x,y=pos
	w,h=sze
	return [rotat(p,pos,rot) for p in [(x-w/2,y-h/2),(x+w/2,y-h/2),(x+w/2,y+h/2),(x-w/2,y+h/2)]]
	
class Robotito:
	def __init__(self,propiedades,pos=(100,100),rot=pi/2):
		self.prop=propiedades
		self.parseXML()
		self.rad=sqrt(2)*self.sze/2.
		self.x,self.y=pos
		self.vx=self.vy=0
		self.vel=self.vrot=0
		self.rot=rot+pi/randint(10,30)*(random()>.5 and 1 or -1)
		self.cubito=Cube(self.sze,(0,0,0),(self.color[0]/255.,self.color[1]/255.,self.color[2]/255.))
		for name,lfw in self.linefs.items(): exec('self.%s = lfw'%name)
		for name,tm in self.teles.items(): exec('self.%s = tm'%name)
		self.clk5ms=clk5ms
		self.prop.init(self)
	
	def parseXML(self):
		lfs={}
		teles={}
		desc = minidom.parseString(self.prop.descripcion)
		for lf in desc.getElementsByTagName("LineFollower"):
			if lf.hasAttribute("pos"): pos=map(lambda x:float(x), lf.getAttribute("pos").split(","))
			else: pos=(0,0)
			lfs[lf.getAttribute("name")]=LineFollower(*pos)
		for tm in desc.getElementsByTagName("Telemetro"):
			if tm.hasAttribute("pos"): pos=map(lambda x:float(x), tm.getAttribute("pos").split(","))
			else: pos=(0,0)
			if tm.hasAttribute("angulo"): ang=radians(float(tm.getAttribute("angulo")))
			else: ang=0
			if tm.hasAttribute("distancia"): lon=float(tm.getAttribute("distancia"))
			else: lon=16
			teles[tm.getAttribute("name")]=Telemetro(pos,ang,lon)
		if desc.firstChild.hasAttribute("color"): self.color=map(int,desc.firstChild.getAttribute("color").split(","))
		else: self.color=(255,255,255)
		if desc.firstChild.hasAttribute("tamanio"): self.sze=float(desc.firstChild.getAttribute("tamanio"))
		else: self.sze=10
		self.linefs=lfs
		self.teles=teles
	
	def pos(self):
		return (self.x,self.y)
	
	def enviarComandoDeVelocidad(self,l,r):
		l=float(-l)/FPS
		r=float(-r)/FPS
		self.vel=(l+r)/2.
		self.vrot= l!=r and (l-r)/(self.sze) or 0
		
	def render(self):
		self.cubito.position=(self.x,self.sze/2.,self.y)
		self.cubito.rot=self.rot
		self.cubito.render()
		#drawPoly(screen,self.color,rotrect(screen,(self.x,self.y),(self.sze,self.sze),self.rot),0)
		#drawLine(screen,(255,0,0), rotat((self.x+self.sze/2,self.y-self.sze/2),(self.x,self.y),self.rot), rotat((self.x+self.sze/2,self.y+self.sze/2),(self.x,self.y),self.rot),3)
		#for n,lf in self.linefs.items(): drawCirc(screen,(200,200,200),i(*rotat((self.x+lf.x,self.y+lf.y),(self.x,self.y),self.rot)),3)
		#for n,tm in self.teles.items(): drawALine(screen,(255,120,120), rotat((self.x+tm.x,self.y+tm.y),(self.x,self.y),self.rot), rotat((self.x+tm.x+tm.lon*cos(tm.ang),self.y+tm.y+tm.lon*sin(tm.ang)),(self.x,self.y),self.rot))
	
	def logic(self,oponentes=[]):
		self.prop.comportamiento(self)
		self.vx=self.vel*cos(self.rot)
		self.vy=self.vel*sin(self.rot)
		self.rot+=self.vrot
		self.x+=self.vx
		self.y+=self.vy
		for lfname,lf in self.linefs.items():
			x,y=rotat((self.x+lf.x,self.y+lf.y),(self.x,self.y),self.rot)
			lf.dato= 73/2. < sqrt((x)**2+(y)**2) < 77/2.
		for tmname,tm in self.teles.items():
			x1,y1=rotat((self.x+tm.x,self.y+tm.y),(self.x,self.y),self.rot)
			x2,y2=rotat((self.x+tm.x+tm.lon*cos(tm.ang),self.y+tm.y+tm.lon*sin(tm.ang)),(self.x,self.y),self.rot)
			l=sqrt((x1-x2)**2+(y1-y2)**2)
			a=tm.ang+self.rot
			tm.dato=0
			cont=True
			for i in xrange(100):
				if not cont: break
				for oponente in oponentes:
					if (x1+cos(a)*i*l/100-oponente.x)**2+(y1+sin(a)*i*l/100-oponente.y)**2 < oponente.rad**2:
						tm.dato=(157-80)*i/100.+80 #para tirar entre 80 y 157
						cont=False


class LineFollower:
	def __init__(self,x,y):
		self.x=x
		self.y=y
		self.dato=False
		
class Telemetro:
	def __init__(self,pos,ang,lon):
		self.x,self.y=pos
		self.ang=ang
		self.lon=lon
		self.dato=0

class Cajita:
	def __init__(self,rad=6):
		self.x=0
		self.y=0
		self.rad=rad
	def logic(self,oponentes=[]):
		self.x,self.y=pygame.mouse.get_pos()
	def blit(self,screen):
		drawCirc(screen,(0,100,0),i(self.x,self.y),int(self.rad))


white=(1.,1.,1.,1.)
glEnable(GL_LIGHTING)
#glEnable(GL_CULL_FACE)
glEnable(GL_DEPTH_TEST)
glEnable(GL_NORMALIZE)
#glLightfv(GL_LIGHT1, GL_DIFFUSE, white)
#glLightfv(GL_LIGHT1, GL_SPECULAR, white)
#glLightfv(GL_LIGHT1, GL_AMBIENT, (0.,0.,0.,1.))
#glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, (0,1,0))
#glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, 128)
#glLightfv(GL_LIGHT2, GL_DIFFUSE, white)
#glLightfv(GL_LIGHT2, GL_SPECULAR, white)
#glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, (1,0,1))
glEnable(GL_LIGHT0)
glEnable(GL_LIGHT1)
glEnable(GL_COLOR_MATERIAL)
glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE)
glMaterialiv(GL_FRONT_AND_BACK,GL_SPECULAR,(1,1,1,1))

#clk=Clock()
#c=Cube(10,(0,0,0),(0.,1.,0.,1.))
#ang=0

import nuevo
import viejo
import rapido
import lento
import muerto

def onstart():
	global r1,r2,cancha,cosas
	r1=Robotito(lento,pos=(10,0),rot=pi/2)
	r2=Robotito(rapido,pos=(-10,0),rot=-pi/2)
	cancha=Cancha()
	#o=Cajita()
	cosas=[r1,r2,cancha]
onstart()
zoom=1.
over=0

while run:
	#screen.fill((150,150,255))
	#screen.blit(bg,(0,0))
	
	#o.update()
	r1.logic([r2])
	r2.logic([r1])
	
	#Collision!
	fric=.6
	d=dist(r1.pos(),r2.pos())
	if d<r1.rad+r2.rad: #crash
		r1.x+=r2.vx*random()+fric
		r1.y+=r2.vy*random()+fric
		r2.x+=r1.vx*random()+fric
		r2.y+=r1.vy*random()+fric
		#r1.x-=r1.vx*fric
		#r1.y-=r1.vy*fric
		#r2.x-=r2.vx*fric
		#r2.y-=r2.vy*fric
		d=dist(r1.pos(),r2.pos())
		ang=atan2(r1.x-r2.x,r1.y-r2.y)+pi
		cant=(r1.rad+r2.rad-d)/2.
		#if ang>pi/2 and ang<3*pi/2: cant*=-1
		r1.x-=cos(ang)*cant
		r2.x+=cos(ang)*cant
		r1.y-=sin(ang)*cant
		r2.y+=sin(ang)*cant
	#r1.blit(screen)
	#r2.blit(screen)
	#o.blit(screen)

	mouseX,mouseY=pygame.mouse.get_pos()
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
	glLoadIdentity()
	glRotatef(50+over+(2/zoom)*20*(float(mouseY)-H/2)/H,1,0,0)
	glRotatef((2/zoom)*25*(float(mouseX)-W/2)/W,0,1,0)
	glTranslatef(0,-50*zoom,(-50+over)*zoom)

	for cosa in cosas: cosa.render()

	pygame.display.flip()
	
	key = pygame.key.get_pressed()
	if key[K_UP] and zoom>.5: zoom*=0.95
	elif key[K_DOWN] and zoom<5: zoom*=1.05
	if key[K_RIGHT] and over<50: over+=1
	elif key[K_LEFT] and over>0: over-=1
		
	for e in pygame.event.get():
		if e.type==QUIT: run=False
		elif e.type==KEYDOWN and e.key in [K_r,K_SPACE]:
			onstart()
			
	clk.tick(FPS)
	


"""
while True:
	clk.tick(30)
	key = pygame.key.get_pressed()
	for event in pygame.event.get():
		if event.type == QUIT or key[K_ESCAPE]:
			pygame.quit(); sys.exit()
		elif key[K_UP]: c.position=(c.position[0]+3,c.position[1],c.position[2])
		elif key[K_DOWN]: c.position=(c.position[0]-3,c.position[1],c.position[2])
	
	Draw([c,cancha])
"""
