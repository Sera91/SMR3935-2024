/* connect mpi processes to a ring and send a message around */

#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
  /* Variables:
   ! me   : rank of current process
   ! prev : rank of process that we are receiving data from
   ! next : rank of process that we are sending data to
   ! mesg : storage for message data
   ! tmp  : holding buffer to incoming data
   ! odd  : flag indicating odd/even rank
   */
  int me, ncpu, prev, next, mesg, i, tmp, odd;

  /* set up MPI environment */
  i = MPI_Init(&argc,&argv);
  if (i != MPI_SUCCESS) {
    puts("problem initializing MPI");
    return 1;
  }

  MPI_Comm_size(MPI_COMM_WORLD,&ncpu);
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  /* this method only works with an even number of CPUs */
  if (ncpu & 0x1) {
    if (me == 0)
      puts("This program requires an even number of CPUs");

    MPI_Finalize();
    return 2;
  }

  /* determine previous and next rank and odd/even status */
  prev = me - 1;
  if (prev < 0) prev = ncpu - 1;

  next = me + 1;
  if (next == ncpu) next = 0;
  odd = me & 0x1;

  /* at the beginning rank 0 holds the message all others have nothing */
  if (me == 0) {
      mesg = 1;
  } else {
      mesg = 0;
  }

  /* run loop to communicate the message buffer from one rank to the next.
   ! to use blocking send/recv without causing a deadlock we alternate the
   ! order of send and receive between odd and even ranks */

  for (i=0; i < 20; ++i) {
    /* odd is sending first and receiving later */
    if (odd) {
      MPI_Send(&mesg,1,MPI_INT,next,0,MPI_COMM_WORLD);
      MPI_Recv(&tmp,1,MPI_INT,prev,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    } else {
      MPI_Recv(&tmp,1,MPI_INT,prev,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
      MPI_Send(&mesg,1,MPI_INT,next,0,MPI_COMM_WORLD);
    }
    /* communication complete for this step. copy incoming buffer to message */
    mesg = tmp;
    if (mesg != 0) printf("step %d: rank %d has message\n",i,me);
  }

  /* shut down MPI environment */
  MPI_Finalize();
  return 0;
}
