#!/usr/bin/env python

import sys
from OpenGL.GL import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
import Tango
from math import sin,cos

class EventListener:
	def __init__(self,typ,callback):
		self.type=typ
		self.callback=callback

class Line:
	def __init__(self,x0,y0,x1,y1,style):
		self.x0,self.y0,self.x1,self.y1,self.style=x0,y0,x1,y1,style

class Lines:
	def __init__(self,pntls,style):
		self.points,self.style=pntls,style
	
class Sprite:	
	def __init__(self):
		self.x=0
		self.y=0
		self.rotation=0
		self.scale=1
		self.curx=0
		self.cury=0
		self.objects=[]
		self.linestyle={"color":Tango.Aluminium6, "width":2.}
		self.lines=[]
		self.is_lines=False
		self.eventListeners=[]
	
	def add(self,obj):
		self.objects.append(obj)
	
	def addEventListener(self,typ,callback):
		self.eventListeners.append(EventListener(typ,callback))
	
	def beginLines(self):
		self.is_lines=True
	
	def endLines(self):
		if self.is_lines:
			self.is_lines=False
			self.add(Lines(self.lines,self.linestyle.copy()))
			self.lines=[]
	
	def moveTo(self,x,y):
		self.curx,self.cury=x,y
	
	def lineTo(self,x,y):
		if self.is_lines: self.lines.append((self.curx,self.cury,x,y))
		else: self.objects.append(Line(self.curx,self.cury,x,y,self.linestyle.copy()))
		self.curx,self.cury=x,y
	
	def lineStyle(self,color=None,width=-1):
		if color: self.linestyle["color"]=color
		if width>=0: self.linestyle["width"]=width









class Bananas:
	def __init__(self,scene):
		self.scene=scene
		self.title="Hello"
		self.FPS=40
		self.frame=0
		self.bgcolor=(239,235,231)
		self.renderOnFrame=True
		self.eventListeners={"EnterFrame":[]}
		glutInit(sys.argv)
		glutCreateWindow(self.title)
		glutReshapeWindow(640, 480)
		self.connectEventListeners(self.scene)
		#glutInitWindowSize(640, 480)
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
		glEnable(GL_LINE_SMOOTH)
		glEnable(GL_POINT_SMOOTH)
		glEnable(GL_BLEND)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST)
		glHint(GL_POINT_SMOOTH_HINT, GL_NICEST)
		glClearColor(*map(lambda x:x/255.,self.bgcolor+(0,)))
		glutReshapeFunc(self.reshape)
		glutKeyboardFunc(self.keyboard)
		glutMouseFunc(self.click)
		glutMotionFunc(self.mouse)
		glutPassiveMotionFunc(self.mouse)
		glutDisplayFunc(self.display)
		glutTimerFunc(50,self.update,0)
	
	def connectEventListeners(self,sprite):
		for evl in sprite.eventListeners:
			if evl.type=="EnterFrame": self.eventListeners["EnterFrame"].append((evl,sprite))
		for o in sprite.objects:
			if o.__class__.__name__==Sprite.__name__: self.connectEventListeners(o)
	
	def start(self):
		glutMainLoop()
	
	def color(self,r,g,b):
		glColor3f(r/255., g/255., b/255.)
		
	def colorhex(self,h):
		lv=len(h)
		glColor3f(*(int(h[i:i+lv/3], 16) for i in range(0, lv, lv/3)))
	
	def reshape(self,w,h):
		glViewport(0, 0, w, h)
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		if w<=h: gluOrtho2D(-1., 1., -float(h)/w, float(h)/w)
		else: gluOrtho2D(-float(w)/h, float(w)/h, -1., 1.)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity()
	
	def render(self,obj):
		glPushMatrix()
		glScalef(obj.scale,obj.scale,1)
		glRotatef(obj.rotation,0,0,-1)
		glTranslatef(obj.x,obj.y,0)
		for o in obj.objects:
			if o.__class__.__name__==Line.__name__:
				glLineWidth(o.style["width"])
				self.color(*o.style["color"])
				glBegin(GL_LINES)
				glVertex2f(o.x0,o.y0)
				glVertex2f(o.x1,o.y1)
				glEnd()
			elif o.__class__.__name__==Lines.__name__:
				glLineWidth(o.style["width"])
				self.color(*o.style["color"])
				glBegin(GL_LINES)
				for l in o.points:
					glVertex2f(l[0],l[1])
					glVertex2f(l[2],l[3])
				glEnd()
			elif o.__class__.__name__==Sprite.__name__:
				self.render(o)
		glPopMatrix()
	
	def display(self):
		glClear(GL_COLOR_BUFFER_BIT)
		self.render(self.scene)
		glFlush()
	
	def update(self,val):
		self.enterFrame()
		self.frame+=1
		time=glutGet(GLUT_ELAPSED_TIME)
		if self.renderOnFrame: glutPostRedisplay()
		time2=glutGet(GLUT_ELAPSED_TIME)
		glutTimerFunc(int(1000./self.FPS-(time2-time)),self.update,0)
	
	def enterFrame(self):
		for evl,sp in self.eventListeners["EnterFrame"]: evl.callback(sp)
	
	def keyboard(self,key,mouseX,mouseY):
		glRotatef(1.,0,0,-1)
	
	def click(self,button,state,mouseX,mouseY):
		pass
		
	def mouse(self,mouseX,mouseY):
		glRotatef(1.,0,0,-1)

if __name__ == '__main__':
	sc=Sprite()
	sq=Sprite()
	sc.add(sq)
	sq.moveTo(-.5,-.5)
	sq.lineTo(.5,-.5)
	sq.lineTo(.5,.5)
	sq.lineTo(-.5,.5)
	sq.lineTo(-.5,-.5)
	sq.rotation=20
	Bananas(sc)
