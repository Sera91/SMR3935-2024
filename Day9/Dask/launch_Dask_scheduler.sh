#!/bin/bash -l
#SBATCH -A tra24_ictp_np
#SBATCH -p boost_usr_prod
#SBATCH --time 0:40:00       # format: HH:MM:SS
#SBATCH -N 1
#SBATCH --ntasks-per-node=4
#SBATCH --cpus-per-task=1
#SBATCH --gpus-per-node=0
#SBATCH --mem-per-cpu=10000
#SBATCH --job-name=dask-scheduler
#SBATCH --output=scheduler_run.txt
#SBATCH --error=scheduler_run.err



source $HOME/Conda_init.txt

module load profile/deeplrn
module load cuda/11.8
module load gcc/11.3.0
module load openmpi/4.1.4--gcc--11.3.0-cuda-11.8  
module load llvm/13.0.1--gcc--11.3.0-cuda-11.8  
module load nccl/2.14.3-1--gcc--11.3.0-cuda-11.8
module load gsl/2.7.1--gcc--11.3.0-omp



conda activate /leonardo_scratch/large/usertrain/$USER/env/SMR3935


dask-scheduler --scheduler-file $HOME/scheduler.json --port 8889 --dashboard-address 8781

# keep it alive
sleep 36000
