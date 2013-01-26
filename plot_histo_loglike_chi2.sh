#!/bin/bash
filelist=$1
output="loglike_out.txt"
rm -f $output
for file in `cat $filelist`
do
	offset=`echo $file | sed 's/.*offset\([0-9]*\)_.*/\1/'`
    lo=`echo $file | sed 's/.*LO\([0-9]*\)_.*/\1/'`
	freq=`echo "24500+$lo+$offset" | bc`
	ll=`python histo_loglike_chisq_2.py $file`
	echo "$freq $lo $offset $ll" >> $output
done
sort -n --key=1 $output > temp
cp temp $output
