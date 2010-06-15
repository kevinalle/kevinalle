#!/usr/bin/env python

import sys
from OpenGL.GL import *
#from OpenGL.GLE import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
from Tango import Tango
from math import sin,cos
from Sprite import *


class Draw:
	def __init__(self,scene):
		self.scene=scene
		self.title="Hello"
		self.FPS=40
		self.frame=0
		self.bgcolor=(239,235,231)
		glutInit(sys.argv)
		glutCreateWindow(self.title)
		glutReshapeWindow(640, 480)
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
			elif o.__class__.__name__==Sprite.__name__:
				self.render(o)
		glPopMatrix()
	
	def display(self):
		glClear(GL_COLOR_BUFFER_BIT)
		self.render(self.scene)
		glFlush()
	
	def update(self,val):
		self.frame+=1
		time=glutGet(GLUT_ELAPSED_TIME)
		glutPostRedisplay()
		time2=glutGet(GLUT_ELAPSED_TIME)
		glutTimerFunc(int(1000./self.FPS-(time2-time)),self.update,0)
	
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
	Draw(sc)
	glutMainLoop()
