#!/bin/bash
filelist=$1
output="mc_out.txt"
rm -f $output
for file in `cat $filelist`
do
	power=`echo $file | sed 's/.*_p\([0-9]\.*[0-9]*e-[0-9]*\)_.*/\1/'`
	total=`python histo_count.py $file -1`
	s1=`python histo_count.py $file 10`
	s2=`python histo_count.py $file 12`
	s3=`python histo_count.py $file 15`
	s4=`python histo_count.py $file 17`
	echo "$power $s1 $s2 $s3 $s4 $total" >> $output
done
gnuplot << GNUPLOTHERE
set terminal png
set output "mc_out.png"
set title "Normalized Counts Above Cut For Simulated Signals\n Various powers, 1 Channel Single Bin\ 1Hz, 100K noise temp, 20s of data"
set xlabel "Signal Power (in fW)
set ylabel "(Counts-<Counts>)/sqrt(<Counts>)"
set label at screen 0.3,0.3 font ",50" "SIMULATION"
set label at screen 0.3,0.6 font ",50" "SIMULATION"
set label at screen 0.6,0.3 font ",50" "SIMULATION"
set label at screen 0.6,0.6 font ",50" "SIMULATION"
plot "$output" using (\$1*1e15):((\$2-\$6*exp(-10))/sqrt(\$6*exp(-10))) with lines title "Counts 4+ Sigma",\
"$output" using (\$1*1e15):((\$3-\$6*exp(-12))/sqrt(\$6*exp(-12))) with lines title "Counts 4.5+ Sigma",\
"$output" using (\$1*1e15):((\$4-\$6*exp(-15))/sqrt(\$6*exp(-15))) with lines title "Counts 5+ Sigma",\
"$output" using (\$1*1e15):((\$5-\$6*exp(-17))/sqrt(\$6*exp(-17))) with lines title "Counts 5.5+ Sigma"

#plot "$output" using (\$1*1e15):(\$2/(\$6*exp(-10))) with lines title "Counts 10+ Sigma",\
#"$output" using (\$1*1e15):(\$3/(\$6*exp(-12))) with lines title "Counts 12+ Sigma",\
#"$output" using (\$1*1e15):(\$4/(\$6*exp(-15))) with lines title "Counts 15+ Sigma",\
#"$output" using (\$1*1e15):(\$5/(\$6*exp(-17))) with lines title "Counts 17+ Sigma"
#plot "$output" using (\$1*1e15):2 with lines title "Counts 10+ Sigma", "$output" using (\$1*1e15):3 with lines title "Counts 12+ Sigma", "$output" using (\$1*1e15):4 with lines title "Counts 15+ Sigma", "$output" using (\$1*1e15):5 with lines title "Counts 17+ Sigma"
GNUPLOTHERE
