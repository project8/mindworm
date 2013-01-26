#!/bin/bash
filelist=$1
output="mc_out.txt"
rm -f $output
onfirst=0
for file in `cat $filelist`
do
	echo "on file $file"
	power=`echo $file | sed 's/.*_p\([0-9]\.*[0-9]*e-[0-9]*\)_.*/\1/'`
	total=`python histo_count.py $file -1`
	s1=`python histo_count.py $file 6`
	s2=`python histo_count.py $file 7`
	s3=`python histo_count.py $file 8`
	s4=`python histo_count.py $file 9`
	echo "$power $s1 $s2 $s3 $s4 $total" >> $output
	if [[ "$onfirst" -eq "0" ]]
	then
		echo "first!"
		onfirst=1
		sbase1=$s1
		sbase2=$s2
		sbase3=$s3
		sbase4=$s4
	fi
done
echo "plotting.."
gnuplot << GNUPLOTHERE
set terminal png
set output "mc_out.png"
set title "Normalized Counts Above Cut For Simulated Signals\n Combined channels,Various powers,  40Hz, 100K noise temp, 2s of data"
set xlabel "Signal Power (in fW)
set ylabel "(Counts-<Counts>)/sqrt(<Counts>)"
set label at screen 0.3,0.3 font ",50" "SIMULATION"
set label at screen 0.3,0.6 font ",50" "SIMULATION"
set label at screen 0.6,0.3 font ",50" "SIMULATION"
set label at screen 0.6,0.6 font ",50" "SIMULATION"
plot "$output" using (\$1*1e15):((\$2-$sbase1)/sqrt($sbase1)) with lines title "Counts 6+ Sigma",\
"$output" using (\$1*1e15):((\$3-$sbase2)/sqrt($sbase2)) with lines title "Counts 7+ Sigma",\
"$output" using (\$1*1e15):((\$4-$sbase3)/sqrt($sbase3)) with lines title "Counts 8+ Sigma",\
"$output" using (\$1*1e15):((\$5-$sbase4)/sqrt($sbase4)) with lines title "Counts 9+ Sigma"
GNUPLOTHERE
