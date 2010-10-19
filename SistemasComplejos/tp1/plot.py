#!/usr/bin/env python

from pylab import imshow,show,subplot
from sys import stdin

stin=stdin.read().strip()
valmax=max(map(float,stin.split()))
valmin=min(map(float,stin.split()))
inn=stin.split("\n\n")

for case,inp in enumerate(inn):
	subplot("%d1%d"%(len(inn),case+1), frameon=False, xticks=[], yticks=[] )
	imshow(map(lambda x:map(float,x.strip().split()), inp.strip().split("\n")), interpolation="nearest", vmax=valmax, vmin=valmin)
show()
