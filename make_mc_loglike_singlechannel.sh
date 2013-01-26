#!/bin/bash
filelist=$1
output="mc_out.txt"
rm -f $output
for file in `cat $filelist`
do
	power=`echo $file | sed 's/.*_p\([0-9]\.*[0-9]*e-[0-9]*\)_.*/\1/'`
	ll=`python histo_loglike_chisq_2.py $file`
	echo "$power $ll" >> $output
done

