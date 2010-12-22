#!/usr/bin/python

# -----------------------------------------
# ScaLAPACK installer
# University of Tennessee Knoxville
# October 16, 2007
# ----------------------------------------

import sys
import os

srcdir = os.getcwd()+'/script'
sys.path = sys.path+[srcdir]

from framework import Frame
from blas import Blas
from lapack import Lapack
from blacs import Blacs
from scalapack import Scalapack



if __name__ == '__main__':

    frame = Frame(sys.argv[1:])

    if (frame.testing!= 0):
       blas_inst     = Blas();
       lap_inst      = Lapack();
    blacs_inst    = Blacs();
    scal_inst     = Scalapack()

    Frame.resume(frame)
