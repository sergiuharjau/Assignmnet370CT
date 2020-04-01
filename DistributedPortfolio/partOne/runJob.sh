#!/bin/bash
# This script can take multiple parameters
# The first is the executable (required) any others would be passed as paremeter# to that executable

if [ "$1" == "" ]; then
    echo "Positional parameter 1 is empty"
else
mpirun -np 10 -hostfile /etc/pdsh/machines --map-by node $1 $2 $3

fi
