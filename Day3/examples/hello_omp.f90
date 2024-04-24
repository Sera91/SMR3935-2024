
PROGRAM hello
  INTEGER :: nthreads, tid
  INTEGER, EXTERNAL :: omp_get_max_threads, omp_get_num_threads, &
      omp_get_thread_num

  nthreads = 1
  tid = 1

!$  nthreads = omp_get_max_threads();
!$  tid = omp_get_num_threads();

  PRINT*,'Total number of threads: ',nthreads, &
      '  Number of active threads:',tid

  nthreads=1
  tid=0

!$OMP parallel default(none) private(tid) shared(nthreads)    
!$ tid=omp_get_thread_num();
!$ nthreads=omp_get_num_threads();

  PRINT*, 'Hello World from thread: ',tid
  IF (tid == 0) THEN
      PRINT*,'Number of active threads:', nthreads
  END IF
!$OMP end parallel
    
END PROGRAM hello
