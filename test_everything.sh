#!/bin/bash

for file in `ls inputs/*.x`
do
	echo "$file" >> my_results.txt
	echo "$file" >> my_results.txt
	#./sim "$file" < run.txt >> my_results.txt
	./ref_sim "$file" < run.txt >> ref_results.txt
done

diff my_results.txt ref_results.txt
echo "FINITO"

exit
