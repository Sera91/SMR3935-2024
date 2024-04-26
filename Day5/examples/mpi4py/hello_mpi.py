#!/usr/bin/env python

from mpi4py import MPI     # Includes calling MPI_Init()

comm = MPI.COMM_WORLD      # an MPI communicator is a class in mpi4py
size = comm.Get_size()     # MPI_Comm_get_size()
rank = comm.Get_rank()     # MPI_Comm_get_rank()

print("I am rank %d of %d" % (rank, size))
