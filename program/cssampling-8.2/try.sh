#!/bin/bash
# /home/work/yamaguchi/CBPM/original/exp/;

## ARGS
n=5;
body="hige";
cssamp="/home/broom/yamaguchi/Dropbox/program/cssampling-8.2/cssamp";
walk=10000;
sample=10;
p="0.5";
r="0.5";
t="0.1";
m="2";
seed=`seed`;
################################################################################
# optarg
################################################################################
while getopts b:s:S:w:p:r:t:m:n:h OPT; do
    case $OPT in
        b) body=$OPTARG;;
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

echo "### CSSAMP ###";
echo "$cssamp
        -i $body.cnf
        -n $walk
        -s $sample
        -S $seed
        -p $p
        -r $r
        -t $t
        -m $m
        -f $f
        -l $l;";
$cssamp -i $body.cnf \
    -n $walk \
    -s $sample \
    -S $seed \
    -p $p \
    -r $r \
    -t $t \
    -m $m \
    -f $f \
    -l $l

# sleep 2;
# echo "### MEMORY STATUS ###";
# memory=0;
# echo `ps aux | grep cssamp` | awk '{print $4}'
# memory=`ps aux | grep cssamp` | awk '{print $4}';
# memory=1200000000/100*$memory;
# echo $memory;
# echo "### MEMORY STATUS 2 ###";
#  top -bcisSHn 1 | grep -v "top \-bcisSHn 1" | grep -v root | sed 's/^$//g' > _tmp_;
#  l=`cat _tmp_ | wc -l`;
#  n=`expr $l - 8`;
#  echo "n="$n;
#  #tail -n `expr $n + 2` _tmp | head -n `expr $n + 1` | awk '{print $2"\t"$9"\t"$10"\t"$11"\t"$12}';
#  tail -n `expr $n + 1` _tmp_ | head -n `expr $n + 1` | awk '{print $10}' > _tmp2_;
#  memory=`echo "\`head -n 1 _tmp2_\`"`;
#  echo $memory;
#  echo "used memory";
#  echo "scale=3; 120000 * $memory" | bc;
#  echo "kB";
#  rm -f _tmp_;
#  rm -f _tmp2_;

gprof ./cssamp gmon.out > less_$body
echo "### MOVE ###";
rm -f ./$body.db;
#rm -f ./$body.prb;
#rm -f ./$body.sli;
#mv ./$body.cnf ./data/;
#mv ./$body.txt ./log/;
mv ./less_$body ./less/;

