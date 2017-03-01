#!/bin/bash

for f in  Instance/*.dat
do
	echo "Processing" $f
	./main $f -q
done
