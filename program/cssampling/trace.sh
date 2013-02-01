#!/bin/bash
#g++ -Wall -c -g cssamp.cpp;
#g++ -Wall -o cssamp cssamp.o -lm;
make clean all
./cssamp -i s5.cnf -s 100;
mtrace ./cssamp trace.txt > trace.log


