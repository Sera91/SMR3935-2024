
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <sys/time.h>

/* internal variables and functions for timer */
static double get_time(void)
{
  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv,&tz);
  double systime = tv.tv_sec;
  systime += ((double) tv.tv_usec)/1000000.0;
  return systime;
}

static double func(double x)
{
    return 4.0/(1.0+x*x);
}

int main( int argc, char *argv[] )
{
    const double pi_known = 3.141592653589793238462643;

    /*
     * mypi      - the area under the curve calculated by a single process
     * sumf      - intermediate sum of slices
     * x         - x value at the center of a slice
     * tstart/tstop - for timing measurements
     * argstr    - command line argument string
     * n         - number of rectangless in which to divide the curve
     * i         - loop counter on a given node
     */

    double mypi,h,sumf,x,tstart,tstop;
    int n, i;

    if (argc < 2) {
       fprintf(stderr, "Usage: %s <intervals> "
               "(with intervals > number processes)\n", argv[0]);
       return 1;
    }

    n=atoi(argv[1]);
    if (n < 1) {
       fprintf(stderr, "Usage: %s <intervals> "
               "(with intervals > number processes)\n", argv[0]);
       return 1;
    }

    printf("Integration with %d intervals\n", n);

    /* get time stamp at starting point */
    tstart = get_time();

    /* compute part */
    h    = 1.0 / (double) n;
    sumf = 0.0;
    for (i = 0; i < n; ++i) {
        x = h * ((double)i + 0.5);
        sumf += func(x);
    }
    mypi = h * sumf;

    /* get time stamp at completion */
    tstop = get_time();

    printf("pi estimate= %.16f, Error is %.16f\n",
           mypi, fabs(mypi - pi_known));

    printf("time used: %g\n", tstop-tstart);

    return 0;
}
