# SMR3935-2024
Official repo for the ICTP/School on Parallel Programming and Parallel Architecture for High Performance Computing


To connect HPC systems on CINECA:

ssh -o "StrictHostKeyChecking=no" -o "UserKnownHostsFile=/dev/null" -o "LogLevel ERROR" $USER@login.g100.cineca.it

ssh -o "StrictHostKeyChecking=no" -o "UserKnownHostsFile=/dev/null" -o "LogLevel ERROR" $USER@login.leonardo.cineca.it

$USER is your account on CINECA

The horrible string '-o "StrictHostKeyChecking=no" -o "UserKnownHostsFile=/dev/null" -o "LogLevel ERROR"' it is needed to avoind the annoying error message about know_host file
