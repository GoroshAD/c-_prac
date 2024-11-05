#! /bin/bash

for i in 250 3000 5750 8500 11250 14000 16750 19500 22250
    do
    for j in 15 180 345 510 675 840 1005 1170 1335 1500 
        do
        ./main $i $j 1
    done
done