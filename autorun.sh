#!/bin/bash

batch=$1

if [ -z "$1" ]; then
  echo "Check arguments"
  exit
fi

str1="./data/weights"
if [ $batch -le 9 ]
then
    str2="00"$batch
fi
if [ $batch -ge 10 ]
then
    str2="0"$batch
fi
if [ $batch -ge 100 ]
then
    str2=$batch
fi
str3="_i"
str=$str1$str2$str3

lines=`wc -l $str`
lines=$(echo $lines | cut -d' ' -f 1)

for i in $(seq 1 $lines ); do
  eval ./main $batch $i
done
