#!/bin/bash
#given a file with a list of powerline output files
#generates plots of raw power over frequency
if [ $# -lt 1 ]
then
	echo "usage: make_power_plot.sh (file with list of powerline output files)"
	exit 0
fi
filelist=$1
#estimate the LF receiver shape
#the averaging method doesn't actually work very well at all,
#and knowing the LF receiver shape isn't all that important, so
#punt this for now
lf_stage_file="work/lf_stage.txt"
python average_power_spectra.py $filelist > $lf_stage_file
plotfile="work/temptoplot.gnuplot"
plotfilesigma="work/temptoplotsigma.gnuplot"
mkdir -p work
prefix=`basename $filelist | sed 's/\(.*\)\..*/\1/'`
firstplot="$prefix""_rawpowerplot.png"
sigmaplot="$prefix""_stdev_over_meanplot.png"
echo "unset key" > $plotfile
echo "set terminal png size 1024,768 lw 2" >> $plotfile
echo "set output \"$firstplot\"" >> $plotfile
echo "set multiplot layout 2,1 title \"Raw Power Spectra from 1/16/2013 Run\"" >> $plotfile
echo "set ylabel \"Power (dBm)\"" >> $plotfile
echo "set xlabel \"Frequency (MHz)\"" >> $plotfile
echo "set title \"Channel 1\"" >> $plotfile
echo "plot \\" >> $plotfile
echo "unset key" > $plotfilesigma
echo "set terminal png size 1024,768 lw 2" >> $plotfilesigma
echo "set output \"$sigmaplot\"" >> $plotfilesigma
echo "set multiplot layout 2,1 title \"Standard Deviation over Mean Power from 1/16/2013 Run\"" >> $plotfilesigma
echo "set ylabel \"Ratio\"" >> $plotfilesigma
echo "set xlabel \"Frequency (MHz)\"" >> $plotfilesigma
echo "set title \"Channel 1\"" >> $plotfilesigma
echo "plot \\" >> $plotfilesigma
start="0"
for file in `cat $filelist`
do
	if [[ "$start" -eq 0 ]]
	then
		start="1"
	else
		echo ",\\" >> $plotfile
		echo ",\\" >> $plotfilesigma
	fi
	lo=`echo $file | sed 's/.*_LO\([0-9]*\)_2013.*/\1/'`
	tfname="work/clipped_$lo.txt"
	#combine this file with the lo shape, and clip off the first and last 10 MHz
	paste $file $lf_stage_file | head -n -412 | tail -n +410 > $tfname
	echo "\"$tfname\" using (\$1+$lo+24500):(10.0*log10(\$2)) with lines\\" >> $plotfile
	echo "\"$tfname\" using (\$1+$lo+24500):(\$4/\$2) with lines\\" >> $plotfilesigma
done
echo " " >> $plotfile
echo " " >> $plotfilesigma
echo "set title \"Channel 2\"" >> $plotfile
echo "set title \"Channel 2\"" >> $plotfilesigma
echo "plot \\" >> $plotfile
echo "plot \\" >> $plotfilesigma
start="0"
for file in `cat $filelist`
do
	if [[ "$start" -eq 0 ]]
	then
		start="1"
	else
		echo ",\\" >> $plotfile
		echo ",\\" >> $plotfilesigma
	fi
	lo=`echo $file | sed 's/.*_LO\([0-9]*\)_2013.*/\1/'`
	tfname="work/clipped_$lo.txt"
	echo "\"$tfname\" using (\$1+$lo+24500):(10.0*log10(\$3)) with lines\\" >> $plotfile
	echo "\"$tfname\" using (\$1+$lo+24500):(\$5/\$3) with lines\\" >> $plotfilesigma
done
echo " " >> $plotfile
echo " " >> $plotfilesigma
echo "unset multiplot" >> $plotfile
echo "unset multiplot" >> $plotfilesigma
#make the plot
gnuplot $plotfile
gnuplot $plotfilesigma
#plotfile2="work/temptoplot2.gnuplot"
#now make the plot so that it divides out the shape
#cat $plotfile | sed 's/\$2/\$2\/\$7/g' | sed 's/\$3/\$3\/\$8/g' | sed 's/rawpowerplot/power_over_lo_shape/' > $plotfile2
#and make it again
#gnuplot $plotfile2
