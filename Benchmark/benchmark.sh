#!/bin/bash

iteration_per_instance=2
output_directory=FifteenKnapsacks
program=../Multi3BKP/main
program_type=main.all
small_pause=40
big_pause=80
regex=20_1.dat
timeout=7200
it=0
number_of_timeouts=0

mkdir $output_directory
for filename in ../MultiInstance/$output_directory/$regex; do
	#if [ $((it)) -ne 0 ]; then
	#	echo "Sleep $big_pause seconds to cool down the system"
	#	sleep $big_pause
	#fi;
	number_of_timeouts=0
	echo $filename
	last_part_of_path=`basename -s .dat $filename`
	echo $last_part_of_path
	output=$output_directory/$last_part_of_path.$program_type.csv
	
	if [ ! -e "$output" ]; then
		echo "create $output"
		echo "User.Time, CPU.Time, Objval, Timeout.Reached" > $output
	fi;
		for (( i=1; i<=iteration_per_instance; i++ ))
		do
			echo "Starting command: $program $filename -t $timeout -b -x -y -z"
			out=`$program $filename -t $timeout -b -x -y -z`
			return_value=$?;
			if [ $return_value -eq 124 ]; then
				number_of_timeouts=$(( number_of_timeouts+1 ))
				echo "Timeout Reached. $output not successful";
				out_array=(`echo $out | tr -d ","`)
				echo "$out, Yes" >> $output
				#if [ $number_of_timeouts -eq 4 ]; then
				#	break;
				#fi;
			elif [ $return_value -eq 1 ]; then
				echo "An error occured. Maybe no solution was found";
				break;
			else
				echo "$out, No" >> $output
				echo "$output iteration $i. Done successfully in $out"
			fi;
			echo "Sleep $small_pause seconds to cool down processor"
			sleep $small_pause;
		done;
	
	it=$((it + 1))
done;
