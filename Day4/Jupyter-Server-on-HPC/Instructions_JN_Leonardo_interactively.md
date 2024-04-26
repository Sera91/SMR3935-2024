#Run tmux session
tmux new -s jupyter


#Run interactive node 

srun  -A tra24_ictp_np -p boost_usr_prod --nodes=1 --ntasks-per-node=4 --cpus-per-task=2 --gpus-per-node=4  --time=01:00:00 --pty bash -i


#Commands to launch on the Computing node

From the terminal you can pass the following commands:

node=$(hostname -s)
echo $node 
module load profile/deeplrn
module load cuda/11.8
module load gcc/11.3.0
module load nccl
module load llvm
module load gsl

conda activate /leonardo/pub/userexternal/sdigioia/sdigioia/env/Gabenv

jupyter-notebook --no-browser --ip=${node}

 
#On your Local machine
On the local machine run:

ssh -N -f -L 8888:lrdn1884:8888 sdigioia@login.leonardo.cineca.it

# What to do after finishing the session with jupyter server on leonardo

 1. Search for tcp connection still opened
 2. Print the PID associated to the tcp connection opened on CINECA cluster
 3. Close the connection on local machine

To search which tcp connections are opened you can use the command:
$ tail -10 /etc/services

to print the PID type the following command on the local terminal:

$ fuser 8888/tcp
(this command will print the PID of the process listening the port in the format "8888/tcp:   PID")


To kill the job (and the associated tcp connection) type:
$ kill -9 PID



