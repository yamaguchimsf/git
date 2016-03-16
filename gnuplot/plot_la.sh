#!/bin/bash

FILE=$1
TITLE=$2
START=$3
END=$4

Usage(){
    echo "Usage: ./plot_la.sh sadcfile pngfilename starttime endtime"
}
### value check
if [ $# -ne 4 ]; then
    Usage
    exit 1
fi

### parse sadc to input format
echo "make basefile..."
rm -f $(basename $1).data2
LC_ALL=C sar -q -f $1 -s $3 -e $4 > $(basename $1).data
while read line
do
    if [[ "$line" =~ ^Average ]]; then
        break
    else
        echo "$line" >> $(basename $1).data2
    fi 
done < $(basename $1).data
mv $(basename $1).data $(basename $2).txt

echo "make graph..."
### make CPU graph using GNUPLOT
INPUT=$(basename $1).data2
gnuplot << EOF
set key right top
set terminal png size 700, 350
set output "$2"
set xdata time
set style fill solid
set timefmt "%H:%M:%S"
set tics font 'Arial,8'
set xlabel "$(basename $2)"
set ylabel "load-average"
set y2tics border
set style line 1 linetype 1 pointtype 0 linewidth 1 linecolor 1
set style line 2 linetype 2 pointtype 0 linewidth 1 linecolor 3
set style line 3 linetype 3 pointtype 0 linewidth 1 linecolor 4
set style line 4 linetype 4 pointtype 0 linewidth 1 linecolor 5
plot ["$3":"$4"][:] '$INPUT' using 1:4  t "load-average(1min)"  w lines linestyle 3, \
    '$INPUT' using 1:5  t "load-average(5min)" w lines linestyle 2, \
    '$INPUT' using 1:6  t "load-average(15min)" w lines linestyle 1 
EOF
rm -f $(basename $1).data2
