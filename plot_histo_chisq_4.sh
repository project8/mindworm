#!/bin/bash
#makes a plot of a histogram with associated chi square degree 4 line
#usage: plot_histo_chisq_2.sh (inputfile) (outputplot)

file=$1
output=$2
title=$3
totalcounts=`python histo_count.py $file -1`
echo "total counts is $totalcounts"

gnuplot << GNUPLOTCOMMANDSHERE
set terminal png
set output "$output"
set title "$title"
set xlabel "Power (sigma)"
set ylabel "Counts"
set log y
set yrange [0.8:*]
plot "$file" using 1:2 title "Measured", (4.0)*($totalcounts)*(0.025)*x*exp(-2.0*x) lt 3 lw 2 title "Predicted" 
GNUPLOTCOMMANDSHERE
