#!/bin/bash

## 2013/2/13

##
for i in 1 2 3 4 5
do
  FILE="./mhws${i}.sadc"
  TEST=teragen
  DEV=ssd
  NET=ib
  NAME=${DEV}-${NET}-${TEST}-400G-50-50
  START=16:28:30
  END=16:34:30
  echo "${NAME} ${START} ${END}"
  ./plot_CPU.sh     ${FILE} ${NAME}-cpu.png  ${START} ${END}
  ./plot_disk.sh    ${FILE} ${NAME}-disk.png ${START} ${END} ${DEV}
  ./plot_network.sh ${FILE} ${NAME}-nw.png   ${START} ${END} ${NET}
  ./plot_memory.sh  ${FILE} ${NAME}-mem.png  ${START} ${END}
  ./plot_la.sh  ${FILE} ${NAME}-la.png  ${START} ${END}
  mkdir -p ./image/${TEST}/${i}/${DEV}-${NET}/
  mv ${NAME}-*.png ./image/${TEST}/${i}/${DEV}-${NET}/
  mv ${NAME}-*.txt ./image/${TEST}/${i}/${DEV}-${NET}/
done
