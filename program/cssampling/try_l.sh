#!/bin/bash
# /home/work/yamaguchi/CBPM/original/exp/;

## ARGS
body="hige";
csslearn="/home/broom/yamaguchi/Dropbox/program/cssampling/csslearn";
walk="200000";
sample="100";
update="1000";
ita="0.001";
p="0.5";
r="0.5";
temp="0.1";
seed=`seed`;
O=$body.dat;
################################################################################
# optarg
################################################################################
while getopts b:s:S:w:p:r:t:i:u:h OPT; do
    case $OPT in
        b) body=$OPTARG;;
        s) sample=$OPTARG;;
        S) seed=$OPTARG;;
        w) walk=$OPTARG;;
        p) p=$OPTARG;;
        r) r=$OPTARG;;
        t) temp=$OPTARG;;
        i) ita=$OPTARG;;
        u) update=$OPTARG;;
        *) echo "Usage : ./try.sh";
            echo " -b <file name>: set training data";
            echo " -s [int]      : set #of samples  (default: 100)";
            echo " -S [int]      : set seed         (default: random)";
            echo " -w [int]      : set max walk     (default: 10000)";
            echo " -p [double]   : set walkSAT prob (default: 0.5)";
            echo " -r [double]   : set RW prob      (default: 0.5)";
            echo " -t [double]   : set temparature  (default: 0.1)";
            echo " -i [double]   : set ita          (default: 1.0e-3)";
            echo " -u [int]      : set max iter     (default: 1000)";
            echo " -h            : show this help";
            exit 1;
    esac
done
i="./train/"$body.cnf;
l="./train/"$body.sli;
o="./train/"$body.prb;
O1=$body"_c.dat";
out=$body"_c.txt";
echo "### CSS+MA ###";
echo "$csslearn -i $i
        -o $o
        -l $l
        -s $sample
        -S $seed
        -w $walk
        -p $p
        -r $r
        -t $temp
        -I $ita
        -u $update
        -O $O1 > $out;";
$csslearn -i $i \
    -o $o \
    -l $l \
    -s $sample \
    -S $seed \
    -w $walk \
    -p $p \
    -r $r \
    -t $temp \
    $M \
    -I $ita \
    -u $update \
    -O $O1 > $out;

O2=$body"_s.dat";
out=$body"_s.txt";
echo "### SS+IS ###";
echo "$csslearn -i $i
        -o $o
        -l $l
        -s $sample
        -S $seed
        -w $walk
        -p $p
        -r $r
        -t $temp
        -m
        -I $ita
        -u $update
        -O $O2 > $out;";
$csslearn -i $i \
    -o $o \
    -l $l \
    -s $sample \
    -S $seed \
    -w $walk \
    -p $p \
    -r $r \
    -t $temp \
    -m \
    -I $ita \
    -u $update \
    -O $O2 > $out;

gnuplot <<EOF
load "le.plt"
set output "$body.eps"
plot "$O1" w l linestyle 1 title "CSsampling+MA", \
"$O2" w l linestyle 2 title "SampleSAT+IS"
exit
EOF
rm $body"_c.dat";
rm $body"_s.dat";
gprof csslearn > $body"_pf.txt";
mv $body"_pf.txt" ./prf/;
