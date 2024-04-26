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

# at the beginning rank 0 will hold the messages 1 and 2
mesg1 = 0
mesg2 = 0
if me == 0:
    mesg1 = 1
    mesg2 = 1

for i in range(0,20):
    # send to next and receive from prev
    mesg1 = comm.sendrecv(mesg1, dest=next, source=prev)
    mesg2 = comm.sendrecv(mesg2, dest=prev, source=next)
    if mesg1:
        print("step %d: rank %d has message 1" % (i, me))
    if mesg2:
        print("step %d: rank %d has message 2" % (i, me))
