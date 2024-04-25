#Initialize conda on Leonardo
cp Conda_init.txt $HOME/

source $HOME/Conda_init.txt

#Step 0: load libraries/modules required on leonardo
module purge
module load profile/deeplrn
module load cuda/11.8
module load gcc/11.3.0
module load nccl
module load llvm
module load gsl

#Step 1: Load my environment from the public area 
conda activate /leonardo/pub/userexternal/sdigioia/sdigioia/env/Gabenv


