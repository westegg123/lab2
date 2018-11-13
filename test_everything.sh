#!/bin/bash

for file in `ls inputs/*.x`
do
	echo "$file" >> my_results.txt
	echo "$file" >> my_results.txt
	#./sim "$file" < run.txt >> my_results.txt
	./ref_sim "$file" < run.txt >> ref_results.txt
done

#./sim "$INPUT_DIR" < run.txt > my_results.txt
#./ref_sim "$INPUT_DIR" < run.txt > ref_results.txt
#diff my_results.txt ref_results.txt

exit
