#!/bin/bash
## ARGS
body="hige";
Body="hige";
cssamp="/home/broom/yamaguchi/Dropbox/program/cssampling-8.2/cssamp";
walk=100000;
sample=10;
p="0.5";
r="0.5";
t="0.1";
m="2";
seed=`seed`;
M_FLG="FALSE";
################################################################################
# optarg
################################################################################
while getopts b:B:os:S:w:p:r:t:mh OPT; do
    case $OPT in
        b) body=$OPTARG;;
        B) Body=$OPTARG;;
        o) o=$body.prb;;
        s) sample=$OPTARG;;
        S) seed=$OPTARG;;
        w) walk=$OPTARG;;
        p) p=$OPTARG;;
        r) r=$OPTARG;;
        t) t=$OPTARG;;
        m) M_FLG="TRUE";;
        *) echo "Usage : ./try.sh";
            echo " -[s # sample |S seed]";
            echo " -b [cnf file name]";
            echo " -B [out file name]";
            echo " -o [file name] : set parameter's probability";
            echo " -w [max walk]";
            echo " -p [double]: prob. of walksat";
            echo " -r [double]: prob. of randomwalk";
            echo " -t [double]: tempareture";
            echo " -m         : SampleSAT mode";
            echo " -h         : show this help";
            exit 1;
    esac
done

f=$Body.prb;
if [ "$M_FLG" = "TRUE" ]; then
    M=`echo "-m"`;
    l=$Body.sam;
else
    l=$Body.sli;
fi
echo "### CSSAMP ###";
echo "$cssamp
        -i $body.cnf
        -o $o
        -n $walk
        -s $sample
        -S $seed
        -p $p
        -r $r
        -t $t
        $M
        -f $f
        -l $l;";
$cssamp -i $body.cnf \
    -o $o \
    -n $walk \
    -s $sample \
    -S $seed \
    -p $p \
    -r $r \
    -t $t \
    $M \
    -f $f \
    -l $l;

gprof ./cssamp gmon.out > less_$Body
rm gmon.out
mv ./less_$Body ./less/;

