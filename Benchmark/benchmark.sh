#!/bin/bash

iteration_per_instance=7
output_directory=TwoKnapsack
program=../Multi3BKP/main
program_type=multi


mkdir $output_directory
rm $output_directory/*.csv
for filename in ../MultiInstance/$output_directory/*.dat; do
	echo $filename
	last_part_of_path=`basename -s .dat $filename`
	echo $last_part_of_path
	output=$output_directory/$last_part_of_path.multi.csv
	echo "create $output"
	echo "User.Time, CPU.Time" > $output
	for i in {1..7}; do
		out=`timeout 20m $program $filename -b -x -y -z -w 6`
		if [ $? -eq 124 ]; then
			echo "Timeout Reached. $output not successful";
		else
			echo $out >> $output
			echo "$output iteration $i. Done successfully in $out"
		fi;
	done;
done;
