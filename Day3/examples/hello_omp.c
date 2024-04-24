
#include <stdio.h>

#if defined(_OPENMP)
#include <omp.h>
#endif

int main(int argc, char **argv)
{
    int nthreads, tid;

    nthreads=1;
    tid=0;

#if defined(_OPENMP)
    nthreads = omp_get_max_threads();
    tid = omp_get_num_threads();
#endif

    printf("Total number of threads: %d    number of active threads: %d\n",
           nthreads, tid);

    nthreads=1;
    tid=0;
#if defined(_OPENMP)
#pragma omp parallel default(none) private(tid) shared(nthreads)
    {
        tid=omp_get_thread_num();
        nthreads=omp_get_num_threads();

        printf("Hello World from thread: %d\n",tid);
        if (tid == 0) {
            printf("Number of active threads: %d\n", nthreads);
        }
    }

#endif
    return 0;
}
