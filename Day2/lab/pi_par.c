#include <stdlib.h>
#include <stdio.h>
// MPI header file 
#include <mpi.h>

#define N 10000

int main( int argc, char * argv[] ){

  int npes, me;
  int me_tot = 0, i = 0, offset = 0, rest = 0;
  double w, f_x = 0.0;
  
  // MPI Initialization phase
  MPI_Init( &argc, &argv);
  MPI_Comm_size( MPI_COMM_WORLD, &npes);
  MPI_Comm_rank( MPI_COMM_WORLD, &me);

  w = 1.0 / N;

  /* 
     1D Domain decomposition method 
     int loc_n = N / npes;
     rest = N % npes;
     if( rest && rank < rest ) loc_n += 1;
     else offset = rest;
     
     start = me * n_loc + offset;
     end = start + n_loc;
     for (i = start; i < end; i++ ){
  */

  /*
    Round robin distribution
  */
  for( i = me; i < N; i += npes){
    
    double x = w * ( i - 0.5 );
    f_x += 4 * ( 1.0 / ( 1 + ( x * x ) ) );
  }

  double f_x_tot = 0.0;
  MPI_Reduce( &f_x, &f_x_tot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
  if( !me )printf( "Value of PI is %.8g\n", f_x_tot * w );
  
  // MPI Finalization phase
  MPI_Finalize();
  
  return 0;
}
