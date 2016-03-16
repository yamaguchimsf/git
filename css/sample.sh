#!/bin/bash
## ARGS
body="dice";
cssamp="./bin/cssamp";
walk=10000;
sample=10000;
p="0.5";
r="0.5";
t="0.1";
seed=`seed`;
M_FLG="FALSE";
################################################################################
# optarg
################################################################################
while getopts b:B:os:S:w:p:r:t:mh OPT; do
    case $OPT in
        b) body=$OPTARG;;
        o) o=$body.prb;;
        s) sample=$OPTARG;;
        S) seed=$OPTARG;;
        w) walk=$OPTARG;;
        p) p=$OPTARG;;
        r) r=$OPTARG;;
        t) t=$OPTARG;;
        m) M_FLG="TRUE";;
        *) echo "Usage : ./sample.sh";
            echo " -[s # sample |S seed]";
            echo " -b [cnf file name] ";
            echo " -w [max walk]";
            echo " -p [double]: prob. of walksat";
            echo " -r [double]: prob. of randomwalk";
            echo " -t [double]: tempareture";
            echo " -m         : SampleSAT mode";
            echo " -h         : show this help";
            exit 1;
    esac
done

f=$body.prb;
if [ "$M_FLG" = "TRUE" ]; then
    M=`echo "-m"`;
    l=$body.sam;
else
    l=$body.sli;
fi
out=$body.txt;
echo "### CSSAMP ###";
echo "$cssamp
        -i ./train/$body.cnf
        -o $o
        -n $walk
        -s $sample
        -S $seed
        -p $p
        -r $r
        -t $t
        $M
        -f $f
        -l $l > $out;";
$cssamp -i ./train/$body.cnf \
    -o $o \
    -n $walk \
    -s $sample \
    -S $seed \
    -p $p \
    -r $r \
    -t $t \
    $M \
    -f $f \
    -l $l > $out;

mv ./$l ./result/samp/;
mv ./$f ./result/samp/;
mv ./$out ./result/samp/;
