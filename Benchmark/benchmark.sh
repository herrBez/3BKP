#!/bin/bash

iteration_per_instance=7
output_directory=FiveKnapsacks
program=../Multi3BKP/main
program_type=multi
it=0

mkdir $output_directory
for filename in ../MultiInstance/$output_directory/*.dat; do
	if [ $((it)) -ne 0 ]; then
		echo "Sleep 120 seconds to cool down the system"
		sleep 50
	fi;
	
	echo $filename
	last_part_of_path=`basename -s .dat $filename`
	echo $last_part_of_path
	output=$output_directory/$last_part_of_path.$program_type.csv
	
	if [ ! -e "$output" ]; then
		echo "create $output"
		echo "User.Time, CPU.Time" > $output
		for i in {1..5}; do
			
			out=`$program $filename -t 3600 -w 2 -b -x -y -z`
			return_value=$?;
			if [ $return_value -eq 124 ]; then
				echo "Timeout Reached. $output not successful";
			elif [ $return_value -eq 1 ]; then
				echo "An error occured. Maybe no solution was found";
				break;
			else
				echo $out >> $output
				echo "$output iteration $i. Done successfully in $out"
			fi;
			echo "Sleep 60 seconds to cool down processor"
			sleep 10;
		done;
		killall main
		it=$((it+1))

	fi;
done;
