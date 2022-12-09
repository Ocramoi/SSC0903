#!/bin/bash

for i in $(seq 2 13)
do
    /usr/bin/time -f "%e" sh -c "mpirun -np 4 par $i > /dev/null"
    /usr/bin/time -f "%e" sh -c "./seq $i > /dev/null"
    echo ''
done
