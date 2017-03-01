#!/bin/bash
#execute automatically
make clean
make
for f in  Instance/*.dat
do
	echo "Processing" $f
	./main $f -q
done


for f in Instance/*.dat
do 
	echo "Processing" $f
	./main $f -q -e
done
