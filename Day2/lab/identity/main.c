#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mpi.h>

#define N 8

void print_matrix( double * mat, int n_loc ){

  int i, j;
  
  for( i = 0; i < n_loc; i = i + 1 ){
    for( j = 0; j < N; j = j + 1 ){
      printf("%.3g ", mat[ i * N + j ] );
    }
    printf("\n");
  }
  
}

int main( int argc, char * argv[] ){
  
  double * mat;
  int i, j, count, offset = 0, rest = 0;
  int me, npes, n_loc;

  MPI_Init( & argc, & argv );
  MPI_Comm_size( MPI_COMM_WORLD, &npes );
  MPI_Comm_rank( MPI_COMM_WORLD, &me );

  printf("\n\n");
  
  n_loc = N / npes; 
  rest = N % npes;
  if( me < rest ) n_loc += 1;
  else offset = rest;
  
  mat = (double *) malloc( n_loc * N * sizeof(double) );
  memset( mat, 0, n_loc * N * sizeof(double) );

  for( i = 0; i < n_loc; i++ ){
    int i_g = i + (me * n_loc) + offset;
    for( j = 0; j < N; j++ ){
      if( i_g == j ) mat[ i * N + j ] = 1.0;
      //mat[ i * N + j ] = j + N * i_g;
    }
  }
  
  if( !me ){
    
    print_matrix( mat, n_loc );
    for( count = 1; count < npes; count += 1 ){
      MPI_Recv(mat, N * n_loc, MPI_DOUBLE, count, count, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
      if( count == rest ) n_loc = n_loc - 1;
      print_matrix( mat, n_loc );
    }
  } else MPI_Send( mat, N * n_loc, MPI_DOUBLE, 0, me, MPI_COMM_WORLD);
  
  
  free( mat );

  MPI_Finalize();
  
  return 0;
}
