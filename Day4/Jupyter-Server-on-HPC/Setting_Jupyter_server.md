#Activate conda env

conda activate MYENV

#Install jupyter server 
Follow installation instructions available at: 
https://docs.jupyter.org/en/latest/install/notebook-classic.html

If we use the environment that I created () we can skip this step becausa I have already
installed Jupyter server

#go to your home folder
cd $HOME

#generate config
jupyter notebook --generate-config
(you should see this line in the terminal at this point: Writing default config to: /leonardo/home/usertrain/a08tra81/.jupyter/jupyter_notebook_config.py)

#set port for Jupyter server
#jupyter notebook --NotebookApp.port=8754

#set password
Writing in the terminal this command
$ jupyter notebook password

you will see 
Enter password:

and once you enter the password you should be all set
