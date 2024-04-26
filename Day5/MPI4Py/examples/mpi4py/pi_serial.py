#!/usr/bin/env python

from math import pi
from time import time

tstart = time()

num = 10000000
sum = 0.0
width = 1.0/num

for i in range(0,num):
    x = (float(i) + 0.5) * width
    f_x = 4.0 / (1.0 + x*x)
    sum += f_x
tend = time()

print("Pi is approximately ", sum*width, " versus ", pi)
print("              Error ", (sum*width) - pi)
print("              Time  ", tend - tstart)
