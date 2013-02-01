#!/bin/bash
# /home/work/yamaguchi/CBPM/original/exp/;

## ARGS
body="hige";
csslearn="/home/broom/yamaguchi/Dropbox/program/cssampling/csslearn";
walk="100000";
sample="100";
update="1000";
ita="0.001";
p="0.5";
r="0.5";
temp="0.1";
seed=`seed`;
M_FLG="FALSE";
C_FLG="FALSE";
D_FLG="FALSE";
O=$body.dat;
################################################################################
# optarg
################################################################################
while getopts b:s:S:w:p:r:t:mcdi:u:h OPT; do
    case $OPT in
        b) body=$OPTARG;;
        s) sample=$OPTARG;;
        S) seed=$OPTARG;;
        w) walk=$OPTARG;;
        p) p=$OPTARG;;
        r) r=$OPTARG;;
        t) temp=$OPTARG;;
        m) M_FLG="TRUE";;
        c) C_FLG="TRUE";;
        d) D_FLG="TRUE";;
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
            echo " -m            : samplesat mode   (default: CS sampling mode)";
            echo " -c            : rejection sampling mode  (default: false)";
            echo " -d            : CD mode on CSsamp(default: false)";
            echo " -i [double]   : set ita          (default: 1.0e-3)";
            echo " -u [int]      : set max iter     (default: 1000)";
            echo " -h            : show this help";
            exit 1;
    esac
done
i="./train/"$body.cnf;
l="./train/"$body.sli;
o="./train/"$body.prb;
O=$body.dat;

if [ "$M_FLG" = "TRUE" ]; then
    M=`echo "-m"`;
fi
if [ "$C_FLG" = "TRUE" ]; then
    C=`echo "-c"`;
fi
if [ "$D_FLG" = "TRUE" ]; then
    D=`echo "-d"`;
fi

echo "### CSSLEARN ###";
echo "$csslearn -i $i
        -o $o
        -l $l
        -s $sample
        -S $seed
        -w $walk
        -p $p
        -r $r
        -t $temp
        $M
        $C
        $D
        -I $ita
        -u $update
        -O $O;";
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
    $C \
    $D \
    -I $ita \
    -u $update \
    -O $O;

gprof csslearn > $body"_pf.txt";
mv $body"_pf.txt" ./prf/;
