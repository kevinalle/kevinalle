#!/usr/bin/env python

from pyogl2D import *
from math import sin, cos, pi

sc=Sprite()
sq=Sprite()
sc.add(sq)
sq.lineStyle(color=Tango.ScarletRed3,width=2)
sq.moveTo(1,0)
for i in range(101): sq.lineTo(cos(2*pi*i/100)*((i%4==1 or i%4==2) and .9 or 1),sin(2*pi*i/100)*((i%4==1 or i%4==2) and .9 or 1))
sq.rotation=20
sq.scale=.5
Draw(sc)
