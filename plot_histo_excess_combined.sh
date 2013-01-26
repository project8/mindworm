#!/bin/bash
filelist=$1
cut=9
output="excess_combined_out.txt"
rm -f $output
for file in `cat $filelist`
do
	offset=`echo $file | sed 's/.*offset\([0-9]*\)_.*/\1/'`
    lo=`echo $file | sed 's/.*LO\([0-9]*\)_.*/\1/'`
	total=`python histo_count.py $file -1`
	s1=`python histo_count.py $file $cut`
	freq=`echo "24500+$lo+$offset" | bc`
	echo "$freq $lo $offset $s1 $total" >> $output
done
sort -n --key=1 $output > temp
cp temp $output
#plot "excess_combined_out_off.txt" using 1:(($4-200)/sqrt(200)) with lines title "Trap Off", "excess_combined_out_on.txt" using 1:(($4-200)/sqrt(200)) with lines title "Trap On", "excess_combined_out_anti.txt" using 1:(($4-200)/sqrt(200)) with lines title "Trap Anti"
