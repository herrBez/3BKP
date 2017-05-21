#!/bin/bash

iteration_per_instance=7
mkdir OneKnapsack
rm OneKnapsack/*.csv
for filename in ../MultiInstance/OneKnapsack/*.dat; do
	echo $filename
	last_part_of_path=`basename -s .dat $filename`
	echo $last_part_of_path
	output=OneKnapsack/$last_part_of_path.small.csv
	echo "create $output"
	echo "User.Time, CPU.Time" > $output
	for i in {1..7}; do
		out=`timeout 15m ../SmallHull/main $filename -b -x -y -z -w 6`
		if [ $? -eq 124 ]; then
			echo "Timeout Reached. $output not successful";
		else
			echo $out >> $output
			echo "$output iteration $i. Done successfully in $out"
		fi;
	done;
done;
