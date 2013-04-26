#!/bin/bash
if [ $# -lt 2 ]
then
    echo "Usage: histogram_loglike_analyze.sh (background egg) (signal egg)"
    exit 0
fi

bgegg=$1
sigegg=$2

filestub=`basename $sigegg | sed s/\.egg$//`
echo $filestub
bglev="`basename $bgegg`".bg_level

#Measure the background levels
./correline -i $bgegg -a >  $bglev

#Create the histogram for background and signal
./powerline_elektronjager -i $bgegg -b $bglev -c standard.conv -p bg
./powerline_elektronjager -i $sigegg -b $bglev -c standard.conv -p sig

#this gives a large positive score for
countstofit=10000
countsforcand=200
maxx=0;
for offset in 20 30 40 50 60
do
    bgfile="bg_offset"$offset"_convolved_powers_histo.txt"
    sigfile="sig_offset"$offset"_convolved_powers_histo.txt"
#where to I cut to get that many events?
    x1=`python get_histogram_x_with_count.py $bgfile $countstofit`
    startx=`echo $x1 | awk '{print $1}'`
    candx=`python get_histogram_x_with_count.py $sigfile $countsforcand | awk '{print $1}'`
    maxx=`echo "if($candx>$maxx) {print \"$candx\"} else {print \"$maxx\"}" | perl`
#what is the fit of an exponential beyond that cut?
    params1=`./fit_histo $bgfile $x1`
#how likely are background and signal to be produced by that exponential?
    llbg=`python calc_loglike.py $bgfile $startx $params1`
    llsig=`python calc_loglike.py $sigfile $startx $params1`
    stopfreq=`echo $offset+20 | bc`
    thedif=`echo $llsig-$llbg | bc -l`
    echo "$offset MHz to $stopfreq MHz score: $thedif"
done

./powerline_elektronjager -i $sigegg -b $bglev -c standard.conv -p sig -d $maxx
