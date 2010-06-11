#!/usr/bin/env python

import sys
from OpenGL.GL import *
#from OpenGL.GLE import *
from OpenGL.GLU import *
from OpenGL.GLUT import *
from Tango import Tango
from math import sin,cos


class Draw:
	title="Hello"
	FPS=40
	frame=0
	bgcolor=(239,235,231)
	
	def __init__(self):
		glutInit(sys.argv)
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB)
		glutInitWindowSize(640, 480)
		glutCreateWindow(self.title)
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
	
	def display(self):
		glClear(GL_COLOR_BUFFER_BIT)
		
		glLineWidth(1.5)
		self.color(*Tango.ScarletRed2)
		glBegin(GL_LINES)
		glVertex2f(-.5, -.5)
		glVertex2f(.5, .5)
		glEnd()
		
		self.color(*Tango.SkyBlue3)
		glBegin(GL_LINES)
		for i in range(100):
			glVertex2f(0, 0)
			glVertex2f(sin(i/10.), cos(i/10.))
		glEnd()
		
		#circ(0,0,25)
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
	Draw()
	glutMainLoop ()
