#!/bin/bash

for file in `ls inputs/*.x`
do
	echo "Testing: $file"
	echo "$file" > my_results.txt
	echo "$file" > ref_results.txt
	./sim "$file" < run.txt >> my_results.txt
	./ref_sim "$file" < run.txt >> ref_results.txt
	diff my_results.txt ref_results.txt
	rm my_results.txt
	rm ref_results.txt
done

echo "DONE TESTING"

exit
