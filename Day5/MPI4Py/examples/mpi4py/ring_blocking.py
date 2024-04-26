#!/usr/bin/env python

from mpi4py import MPI

comm = MPI.COMM_WORLD
me = comm.Get_rank()
ncpu = comm.Get_size()

if (ncpu % 2) != 0:
    if me == 0:
        print("This program requires an even number of CPUs")
    exit()

# get rank of previous and next process
prev = me - 1
if prev < 0:
    prev = ncpu - 1

next = me + 1
if next == ncpu:
    next = 0

# odd/even status of current rank
odd = me % 2

# at the beginning rank 0 will hold the message
if me == 0:
    mesg = 1
else:
    mesg = 0

newmesg = 0
for i in range(0,20):
    # odd ranks are sending first and receiving later
    if odd:
        comm.send(mesg, next)
        newmesg = comm.recv()
    else:
        newmesg = comm.recv()
        comm.send(mesg, next)
    # communication complete. copy data from incoming buffer to proper location
    mesg = newmesg
    if mesg:
        print("step %d: rank %d has message" % (i, me))
