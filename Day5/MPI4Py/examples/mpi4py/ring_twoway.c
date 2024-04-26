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
   ! tmp  : holding buffer for incoming data
   */
  int me, ncpu, prev, next, mesg1, mesg2, i, tmp1, tmp2;

  /* set up MPI environment */
  i = MPI_Init(&argc,&argv);
  if (i != MPI_SUCCESS) {
    puts("problem initializing MPI");
    return 1;
  }

  MPI_Comm_size(MPI_COMM_WORLD,&ncpu);
  MPI_Comm_rank(MPI_COMM_WORLD,&me);

  /* EDIT: determine previous and next rank to the current rank */
  prev = me - 1;
  if (prev < 0) prev = ncpu - 1;

  next = me + 1;
  if (next == ncpu) next = 0;

  /* EDIT: at the beginning assign the message (a number != 0) to rank 0
   * all other ranks have nothing (= 0) */
  if (me == 0) {
    mesg1 = 1;
    mesg2 = 1;
  } else {
    mesg1 = 0;
    mesg2 = 0;
  }

  /* run loop to communicate the message buffers from one rank to the next.
   to avoid a deadlock we use MPI_Sendrecv() to receive and send at the same time.
   We send mesg1 forwards and mesg2 backwards. */

  for (i=0; i < 20; ++i) {
    /* EDIT: Send message 1 to next and receive from prev. */
    MPI_Sendrecv(&mesg1,1,MPI_INT,next,0,&tmp1,1,MPI_INT,prev,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    /* EDIT: Send message 2 to prev and receive from next. */
    MPI_Sendrecv(&mesg2,1,MPI_INT,prev,0,&tmp2,1,MPI_INT,next,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

    /* communication complete for this step. copy incoming buffers to messages */
    mesg1 = tmp1;
    mesg2 = tmp2;
    if (mesg1 != 0) printf("step %d: rank %d has message 1\n",i,me);
    if (mesg2 != 0) printf("step %d: rank %d has message 2\n",i,me);
  }

  /* shut down MPI environment */
  MPI_Finalize();
  return 0;
}
