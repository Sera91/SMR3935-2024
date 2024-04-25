#!/bin/bash -l
#SBATCH -A tra24_ictp_np
#SBATCH -p boost_usr_prod
#SBATCH --time 1:15:00       # format: HH:MM:SS
#SBATCH -N 1
#SBATCH --ntasks-per-node=2
#SBATCH --cpus-per-task=2
#SBATCH --gpus-per-node=2
#SBATCH --mem-per-cpu=10000
#SBATCH --job-name=Jupylab
#SBATCH --output=jupyter_notebook.txt
#SBATCH --error=jupyter_notebook.err


cd $SCRATCH/SMR-3935/Day4

source $HOME/Conda_init.txt

module load profile/deeplrn
module load cuda/11.8
module load gcc/11.3.0
module load nccl
module load llvm
module load gsl



conda activate /leonardo/pub/userexternal/sdigioia/sdigioia/env/Gabenv


# get tunneling info
XDG_RUNTIME_DIR=""
node=$(hostname -s)
user=$(whoami)
portval=88$(whoami | cut -b 7-9)

# print tunneling instructions jupyter-log
echo -e "
# Note: below 8888 is used to signify the port.
#       However, it may be another number if 8888 is in use.
#       Check jupyter_notebook_%j.err to find the port.

# Command to create SSH tunnel:
ssh  -o \"PreferredAuthentications=keyboard-interactive,password\" -o \"StrictHostKeyChecking=no\" -o \"UserKnownHostsFile=/dev/null\" -o \"LogLevel ERROR\"  -N -f -L  $portval:${node}:$portval ${user}@login.leonardo.cineca.it
# Use a browser on your local machine to go to:
http://localhost:$portval/
"

jupyter-notebook --no-browser --ip=${node} --port=${portval}

# keep it alive
sleep 36000
