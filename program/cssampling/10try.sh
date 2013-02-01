#!/bin/bash

ita="0.0005";
iter="10";
################################################################################
# optarg
################################################################################
while getopts b:i: OPT; do
    case $OPT in
        b) body=$OPTARG;;
        i) ita=$OPTARG;;
        *) echo "Usage : ./try.sh";
            echo " -b <file name>: set file name";
            echo " -i [double]   : set ita (default: 5.0e-3)";
            echo " -h            : show this help";
            exit 1;
    esac
done

i="0";
while test $i -lt $iter
do
    i=`expr $i + 1`;
    f=$body"-"$i".txt";
    ./try.sh -b train20 -i $ita -u 10000 -t 0.1 -r 0.5 -p 0.5 > $f;
done

