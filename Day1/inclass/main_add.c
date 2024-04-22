#include <stdlib.h>
#include <stdio.h>

#ifdef DEBUG
#define N 4
#else
#define N 10000
#endif

void add_mul( int a, int b, int * c, int * d){

  //this is what I mean to implement 
  *(c) = a + b;
  *(d) = a * b;
}

void add_vec( int * a, int * b, int * c)
{

  int i;
  for( i = 0; i < N; i++ ){
    c[i] = a[i] + b[i];
    *( c + i ) = *( a + i ) + *( b + i );
  }
}

int main( int argc, char * argv[] ){

  int a = 2, b = 3, c = 3, d =4;
  int vec_c[N];
  int vec_a[N] = { 2, 2, 2, 2 };
  int vec_b[N] = { 3, 3, 3, 3 }; 
  
  add_mul( a, b, &c ,&d );

  printf("\nc = %d", c );
  
  add_vec( vec_a, vec_b, vec_c);

  printf("\nc_vec[ 3 ] = %d out of a %d size vector\n\n", vec_c[ 3 ], N );
  
  return 0;

}
