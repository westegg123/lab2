#!/bin/bash

cycles=$1

for ((i = 1; i <= cycles; i++))
do
        echo "run 1" >> run.txt
        echo "rdump" >> run.txt
done
echo "quit" >> run.txt

exit
