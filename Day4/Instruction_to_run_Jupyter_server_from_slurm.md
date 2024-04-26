#On Leonardo
pull the more recent version of the Github repo

move to the subfolder Day4 inside the SMR-3935 folder

modify the script "launch_Jupyter.sh"  changing portval value (at line 36) into 88XX where XX are the last two numbers of your user account 


This script will save its output in a file "jupyter_notebook.txt" where you can find the name of the computing node assigned to your job

#On your Local machine
On the local machine run:

ssh -N -f -L 88XX:YOURNODE:88XX sdigioia@login.leonardo.cineca.it

where 
XX are the last two numbers of your user account 
YOURNODE is the name of the node written in the jupyter_notebook.txt

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



