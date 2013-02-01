#!/bin/bash
# /home/work/yamaguchi/CBPM/original/exp/;

## ARGS
n=5;
body="hige";
cssamp="/home/broom/yamaguchi/Dropbox/program/cssampling/cssamp";
walk=1000;
sample=10;
p="0.5";
r="0.5";
t="0.1";
m="2";
seed=`seed`;
bar="1";
################################################################################
# optarg
################################################################################
while getopts b:B:s:S:w:p:r:t:m:n:h OPT; do
    case $OPT in
        b) body=$OPTARG;;
        B) bar=$OPTARG;;
        s) sample=$OPTARG;;
        S) seed=$OPTARG;;
        w) walk=$OPTARG;;
        p) p=$OPTARG;;
        r) r=$OPTARG;;
        t) t=$OPTARG;;
        m) m=$OPTARG;;
        n) n=$OPTARG;;
        *) echo "Usage : ./try.sh";
            echo " -[s # sample |S seed]";
            echo " -b [file name] ";
            echo " -w [max walk]";
            echo " -p [double]: prob. of walksat";
            echo " -r [double]: prob. of randomwalk";
            echo " -t [double]: tempareture";
            echo " -m [int]   : mode 1:SampleSAT, 2:Slice Sampling, 3:Both Sa and Sl";
            echo " -n [int]   : number of people";
            echo " -h         : show this help";
            exit 1;
    esac
done
l=$body.sli;
f=$body.prb;
echo "### MAKE .db FILE ###";
i=1;
while [ $i -le $n ]; do 
    echo "smoke($i)";
    i=`expr $i + 1`;
done > $body.db;
#cat $body.db;

echo "### MAKE .cnf FILE ###"
./smoking.rb -b $body;
h=`head -1 $body.cnf`;
echo $h;

y=0;
while test $y -lt $bar
do
    y=`expr $y + 1`;
    echo "### CSSAMP ###";
    echo "$cssamp
        -i $body.cnf
        -n $walk
        -s $sample
        -S `expr $seed + $y`
        -p $p
        -r $r
        -t $t
        -m $m
        -f $f
        -l $l;";
    $cssamp -i $body.cnf \
        -n $walk \
        -s $sample \
        -S `expr $seed + $y` \
        -p $p \
        -r $r \
        -t $t \
        -m $m \
        -f $f \
        -l $l;
    echo "### SAMPLING $y FINISHED ###";
    echo "### DELETE ###";
    rm -f ./$body.db;
    rm -f ./$body.sli;
    rm -f ./$body.prb;
done

echo "### MOVE###";
mv ./$body.cnf ./data/;
mv ./$body.txt ./log/;


