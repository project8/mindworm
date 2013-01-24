#!/bin/bash
#makes a plot of a histogram with associated chi square degree 2 line
#usage: plot_histo_chisq_2.sh (inputfile) (outputplot)

file=$1
output=$2
totalcounts=`python histo_count.py $file -1`
echo "total counts is $totalcounts"

gnuplot << GNUPLOTCOMMANDSHERE
set terminal png
set output "$output"
set title "Power Distribution"
set xlabel "Power (sigma)"
set ylabel "Counts"
set log y
set yrange [0.8:*]
plot "$file" using 1:2 title "Measured", ($totalcounts)*(0.025)*exp(-x) lt 3 lw 2 title "Predicted" 
GNUPLOTCOMMANDSHERE
