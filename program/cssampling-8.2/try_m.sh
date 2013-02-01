#!/bin/bash
## ARGS
body="dice";
marginal="/home/broom/yamaguchi/Dropbox/program/cssampling-8.2/marginal";
sample=10000;
roop=1;
seed=`seed`;
M_FLG="FALSE";
out="result.txt";
################################################################################
# optarg
################################################################################
while getopts b:s:S:r:h OPT; do
    case $OPT in
        b) body=$OPTARG;;
        s) sample=$OPTARG;;
        r) roop=$OPTARG;;
        S) seed=$OPTARG;;
        *) echo "Usage : ./try_m.sh";
            echo " -[s # sample |S seed]";
            echo " -b [cnf file name] ";
            echo " -r [num of roop] ";
            echo " -h  : show this help";
            exit 1;
    esac
done
o=$body.prb;
a=0;
# calc marginal probability from samples
while test $a -lt $roop
do
    a=`expr $a + 1`;
    l=$body$a.dat;
    out=$body$a.txt;
    seed=`expr $seed + $a`;
    echo "### MARGINAL ###";
    echo "$marginal
        -i $body.cnf
        -o $o
        -s $sample
        -S $seed
        -l $l
        > $out;";
    $marginal -i $body.cnf \
        -o $o \
        -s $sample \
        -S $seed \
        -l $l > $out;
done

# calc average & covariance
a1=$body"1.dat";
a2=$body"2.dat";
a3=$body"3.dat";
a4=$body"4.dat";
a5=$body"5.dat";
out=$body.dat;
./marginal.rb -a $a1 \
    -b $a2 \
    -c $a3 \
    -d $a4 \
    -e $a5 \
    -z $roop > $out;

# print graph
gnuplot <<EOF
load "ma.plt"
set output "$body.eps"
plot "$out" using 1:2:4 with yerrorbar linestyle 1 title "CSsampling+MA" , \
"$out" using 1:2 with lp linestyle 1 notitle, \
"$out" using 1:3:5 with yerrorbar linestyle 2 title "SampleSAT+IS" , \
"$out" using 1:3 with lp linestyle 2 notitle
exit
EOF

# delete temp files
a=0;
rm $out;
while test $a -lt $roop
do
    a=`expr $a + 1`;
    l=$body$a.dat;
    rm $l;
done
gprof ./marginal gmon.out > less_$body
rm gmon.out
#echo "### MOVE ###";
mv ./less_$body ./less/;

