#!/usr/bin/env python

from mpl_toolkits.mplot3d import Axes3D
import matplotlib
from matplotlib import pyplot as plt
from sys import stdin

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

inn=stdin.read().strip().split("\n")

ax.plot_surface(X, Y, Z, rstride=1, cstride=1, cmap=cm.jet)
plt.show()
