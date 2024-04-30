#Initialize conda on Leonardo
after pulling the updated version of the Github repo (git pull) you can copy thefile Conda_init.txt (under folder Day4)  into your home directory ($HOME) and you can source the file to automatically load anaconda:

source $HOME/Conda_init.txt

#Step 0: load libraries/modules required on leonardo

module load profile/deeplrn
module load cuda/11.8
module load gcc/11.3.0
module load nccl
module load llvm
module load gsl

#Step 1: Load my environment from the public area 
conda activate /leonardo/pub/userexternal/sdigioia/sdigioia/env/Gabenv

#Step 2: Deactivate it

conda deactivate

# Step 3: Clone my environment in your local scratch area

You can copy this env in your local scratch area using this command:

conda create -c conda-forge --override-channels -p /leonardo_scratch/large/usertrain/$USER/env/SMR3935  --clone /leonardo/pub/userexternal/sdigioia/sdigioia/env/Gabenv

pip install     --extra-index-url=https://pypi.nvidia.com     cudf-cu11==24.4.* dask-cudf-cu11==24.4.* cuml-cu11==24.4.*     cugraph-cu11==24.4.* cuspatial-cu11==24.4.* cuproj-cu11==24.4.*     cuxfilter-cu11==24.4.* cucim-cu11==24.4.* pylibraft-cu11==24.4.*     raft-dask-cu11==24.4.* cuvs-cu11==24.4.*


conda create -c conda-forge --override-channels -p /leonardo/pub/userexternal/sdigioia/sdigioia/env/Gabenv2  --clone /leonardo_scratch/large/userexternal/sdigioia/CondaEnv/Gabenv
