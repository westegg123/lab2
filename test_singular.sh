#!/bin/bash

input=$1

#rm my_results.txt
#rm ref_results.txt

./sim "$input" < run.txt > my_results.txt
./ref_sim "$input" < run.txt > ref_results.txt

diff my_results.txt ref_results.txt
echo "FINITO"
exit
