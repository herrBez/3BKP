#!/bin/bash

iteration_per_instance=5
max_line=$(($iteration_per_instance+1))
program=../SmallHull/main 
program_type=small.all
small_pause=40
regex=15_*_bigger.dat
timeout=6300

for output_directory in OneKnapsack FifteenKnapsacks FiveKnapsacks TenKnapsacks; do
mkdir $output_directory
for filename in ../MultiInstance/$output_directory/$regex; do
	
	
	number_of_timeouts=0
	echo $filename
	last_part_of_path=`basename -s .dat $filename`
	echo $last_part_of_path
	output=$output_directory/$last_part_of_path.$program_type.csv
	
	if [ ! -e "$output" ]; then
		echo "create $output"
		echo "User.Time, CPU.Time, Objval, Timeout.Reached" > $output
	fi
	
	
	
	for (( i=1; i<=iteration_per_instance; i++ ))
	do
			
			lines=`cat $output | wc -l`
			echo "lines = $lines"
			if [ "$lines" -ge "$max_line" ]; then #break if the 6 lines are reached
				break;
			fi
			
			
			echo "Starting command: $program $filename -t $timeout -o /tmp/output.$last_part_of_path.$i.$program_type.csv -b -x -y -z"
			out=`$program $filename -t $timeout -b -x -y -z`
			return_value=$?;
			if [ $return_value -eq 124 ]; then
				number_of_timeouts=$(( number_of_timeouts+1 ))
				echo "Timeout Reached. $output not successful";
				out_array=(`echo $out | tr -d ","`)
				echo "$out, Yes" >> $output
			elif [ $return_value -eq 1 ]; then
				echo "An error occured. Maybe no solution was found";
				echo "infeasible" >> $output
				break;
			else
				echo "$out, No" >> $output
				echo "$output iteration $i. Done successfully in $out"
			fi;
			echo "Sleep $small_pause seconds to cool down processor"
			sleep $small_pause;
		done;
done;
done;
