#!/usr/bin/env python

from Bananas import *
from math import sin, cos, pi
from random import random

def f(sprite):
	sprite.rotation+=1

sc=Sprite()
sq=Sprite()
sc.add(sq)
sq.lineStyle(color=Tango.ScarletRed3,width=2)
sq.beginLines()
sq.moveTo(.5,0)
for i in range(201):
	if i%4==0: r=random()+.5;
	sq.lineTo(cos(2*pi*i/200)*((i%4==1 or i%4==2) and r or .5),sin(2*pi*i/200)*((i%4==1 or i%4==2) and r or .5))
sq.endLines()
sq.rotation=20
sq.scale=.5
sq.addEventListener("EnterFrame",f)
b=Bananas(sc)
#b.renderOnFrame=False
b.start()
