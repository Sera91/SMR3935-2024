#!/usr/bin/env python

from mpi4py import MPI

comm = MPI.COMM_WORLD
me = comm.Get_rank()
ncpu = comm.Get_size()

# set rank of previous and next process
prev = me - 1
if prev < 0:
    prev = ncpu - 1

next = me + 1
if next == ncpu:
    next = 0

# at the beginning rank 0 will hold the message
mesg = 0
if me == 0:
    mesg = 1

for i in range(0,20):
    # send to next and receive from prev
    mesg = comm.sendrecv(mesg, dest=next, source=prev)
    if mesg:
        print("step %d: rank %d has message" % (i, me))
