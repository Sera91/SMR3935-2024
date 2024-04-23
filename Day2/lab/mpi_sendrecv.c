#include <stdlib.h>
#include <stdio.h>
// MPI header file 
#include <mpi.h>

#define N 10000

int main( int argc, char * argv[] ){

  int npes, me;
  int msg_recv = -1;
  
  // MPI Initialization phase
  MPI_Init( &argc, &argv);
  MPI_Comm_size( MPI_COMM_WORLD, &npes);
  MPI_Comm_rank( MPI_COMM_WORLD, &me);

  int dest, source;
  dest = source = !me;

  printf( "\nI am process %d and content of msg is %d\n", me, msg_recv );
  
  MPI_Sendrecv( &me, 1, MPI_INT, dest, 100, &msg_recv, 1, MPI_INT, source, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE );

  printf( "\nI am process %d and content of msg is %d\n", me, msg_recv );
  
  // MPI Finalization phase
  MPI_Finalize();
  
  return 0;
}
