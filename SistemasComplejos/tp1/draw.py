#!/usr/bin/env python

from pylab import plot,show
from sys import stdin

inn=stdin.read().strip().split("\n")

for case,inp in enumerate(inn):
	plot(*zip(*enumerate(map(float,inp.strip().split()))), color=str(.5-float(case)/(2*len(inn))))

show()
