#include <stdlib.h>
#include <stdio.h>

int main( int argc, char * argv[] ){

  int vec[4] = {0, 1, 2, 3};
  int * p;
  int i;

  for( i = 0; i < 4; i++ ) vec[i] = i;

  vec[ 2 ] = 4;
  //out of bound exmple
  //  vec[ 5 ] = 6;

  p = vec;
  printf("\np[2] = %d, vec[2] = %d\n", p[ 2 ], vec[ 2 ]);

  p = &vec[2];
  printf("\np[0] = %d, vec[2] = %d\n", p[ 0 ], vec[ 2 ]);

  printf("\n%p", p);
  p = p - 1;
  printf("\n%p", p);
  
  printf("\np[0] = %d, vec[2] = %d\n", p[ 0 ], vec[ 2 ]);
  //  p[2] == *(p + 2);
  
  return 0;

}
