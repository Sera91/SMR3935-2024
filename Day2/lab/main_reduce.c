#include <stdlib.h>
#include <stdio.h>
// MPI header file 
#include <mpi.h>

#define N 10

int main( int argc, char * argv[] ){

  int npes, me;
  int me_tot = 0, i = 0;
  int vec[N], vec_tot[N];
  
  // MPI Initialization phase
  MPI_Init( &argc, &argv);
  MPI_Comm_size( MPI_COMM_WORLD, &npes);
  MPI_Comm_rank( MPI_COMM_WORLD, &me);

  // MPI Core phase
  printf("\n Hello world! I am %d out of %d processes...\n", me, npes );

  MPI_Reduce( &me, &me_tot, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if( !me ) printf("\n\nThe result of the sum of all ranks is: %d", me_tot );

  for( i = 0; i < N; i++ ) vec[ i ] = me;

  MPI_Reduce( vec, vec_tot, N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if( !me ) printf("\n\nThe result of the sum on vec_tot at position (N - 1) of all ranks is: %d\n", vec_tot[ N - 1 ] );
  
  // MPI Finalization phase
  MPI_Finalize();
  
  return 0;
}
