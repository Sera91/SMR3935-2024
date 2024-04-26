#!/usr/bin/env python

from mpi4py import MPI
from math import pi
from time import time

tstart = time()

comm = MPI.COMM_WORLD
size = comm.Get_size()     # MPI_Comm_get_size()
rank = comm.Get_rank()     # MPI_Comm_get_rank()

num = 10000000
sum = 0.0
width = 1.0/num

for i in range(rank,num,size):
    x = (float(i) + 0.5) * width
    f_x = 4.0 / (1.0 + x*x)
    sum += f_x
tot_sum = comm.reduce(sum, MPI.SUM, 0)
    
tend = time()

if rank == 0:
    print("Pi is approximately ", tot_sum*width, " versus ", pi)
    print("              Error ", (tot_sum*width) - pi)
    print("              Time  ", tend - tstart)
