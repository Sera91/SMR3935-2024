
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#if defined(_OPENMP)
#include <omp.h>
#endif

/* compute high precision walltime and walltime difference */
static double wallclock(const double * ref)
{
    struct timespec t;
    double ret;

    clock_gettime(CLOCK_REALTIME, &t);
    ret = ((double) t.tv_sec) + 1.0e-9*((double) t.tv_nsec);

    return ref ? (ret - *ref) : ret;
}


/* extremely naiive matrix multiply C(n,o) = A(n,m) * B(m,o) */

void matmul_cpu(double *a, double *b, double *c, int n, int m, int o)
{
    int i,j,k;
    double sum;

    for (i = 0; i < n; ++i) {
        for (j = 0; j < o; ++j) {
            sum = 0.0f;
            for (k = 0; k < m; ++k) {
                sum += a[m*i+k] * b[o*k+j];
            }
            c[o*i+j] = sum;
        }
    }
}


/* parallelize by adding OpenMP directive(s) */

void matmul_omp(double *a, double *b, double *c, int n, int m, int o)

{
    int i,j,k;
    double sum;

#if defined(_OPENMP)
#pragma omp parallel for private(i,j,k,sum)
#endif
    for (j = 0; j < o; ++j) {
        double bcache[o];
        for (k = 0; k < o; ++k)
            bcache[k] = b[o*k+j];
        for (i = 0; i < n; ++i) {
            sum = 0.0f;
            for (k = 0; k < m; ++k) {
                sum += a[m*i+k] * bcache[k];
            }
            c[o*i+j] = sum;
        }
    }
}

int main(int argc, char **argv)
{
    double secs, secs_cpu, speedup;
    double *a, *b, *c1, *c2;
    int n,m,o,i,flag,nthreads;

    if (argc < 1) {
        printf("usage: %s <dim1>\n", argv[0]);
        return 1;
    }

    n = atoi(argv[1]);
    m = atoi(argv[1]);
    o = atoi(argv[1]);

    if (n < 1) {
        printf("dim1 must be > 1\n");
        return 2;
    }
    if (m < 1) {
        printf("dim2 must be > 1\n");
        return 3;
    }
    if (o < 1) {
        printf("dim3 must be > 1\n");
        return 4;
    }
    posix_memalign((void **)&a,64,sizeof(double)*n*m);
    posix_memalign((void **)&b,64,sizeof(double)*m*o);
    posix_memalign((void **)&c1,64,sizeof(double)*n*o);
    posix_memalign((void **)&c2,64,sizeof(double)*n*o);

    /* fill matrix a */
    for (i=0; i < n*m; ++i) {
        a[i] = rand()/(double)RAND_MAX;
    }
    /* fill matrix b */
    for (i=0; i < m*o; ++i) {
        b[i] = rand()/(double)RAND_MAX;
    }
    /* warm up cache */
    matmul_omp(a,b,c2,n,m,o);

    /* run simple CPU version and measure time */
    secs = wallclock(NULL);
    matmul_cpu(a,b,c1,n,m,o);
    secs_cpu = wallclock(&secs);
    printf("time for CPU code: %g s\n",wallclock(&secs));

    /* run multithreaded version and measure time */
#if defined(_OPENMP)
    nthreads = omp_get_max_threads();
#else
    nthreads = 1;
#endif
    secs = wallclock(NULL);
    matmul_omp(a,b,c2,n,m,o);
    secs = wallclock(&secs);
    printf("time for OMP code with %d threads: %g s\nspeedup: %g\n",nthreads,secs,secs_cpu/secs);

    /* check result */
    flag=0;
    for (i=0; i < n*o; ++i) {
        if (fabs((c1[i] - c2[i])/c1[i]) > 0.000002) {
            flag=1;
        }
    }
    if (flag == 0) {
        printf("check OK\n");
    } else {
        printf("check FAIL\n");
    }

    free(a);
    free(b);
    free(c1);
    free(c2);
    return 0;
}
