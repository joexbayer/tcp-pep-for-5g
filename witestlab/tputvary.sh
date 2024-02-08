#!/bin/bash

while IFS=, read -r tput tdiff
do 
	tdiff=${tdiff::-1}	
	#ts= `date +%s.%N`
	#echo "$tdiff,$tput"
	sudo tc class replace dev $(ip route get 10.100.67.7 | grep -oP "(?<=dev )[^ ]+") parent 1: classid 1:3 htb rate "$tput"mbit
	sleep $tdiff
done < "$1"-tput.csv