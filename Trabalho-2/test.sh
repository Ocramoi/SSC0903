#!/bin/bash

for i in $(seq 2 15)
do
    echo N=$i
    echo par
    /usr/bin/time -f "%e" sh -c "mpirun -np 4 par $i > /dev/null"
    echo seq
    /usr/bin/time -f "%e" sh -c "./seq $i > /dev/null"
    echo ''
done
