Link to MPI basic functions documentation: 

https://www.mpi-forum.org/docs/mpi-1.1/mpi-11-html/node182.html

Command line example for taking an interactive node on G100 on the debug partition: 

srun --nodes=1 --ntasks-per-node=48 --cpus-per-task=1 -A tra24_ictp_np_0 --time 00:10:00 --mem=350000MB -p g100_usr_dbg --pty /bin/bash

Command line example for taking an interactive node on Leonardo on the GPU partition: 

srun --nodes=1 --ntasks-per-node=32 --cpus-per-task=1 -A tra24_ictp_np --time 00:20:00 --gres=gpu:4 --mem=490000MB -p boost_usr_prod --pty /bin/bash