#!/bin/bash

batch=$1

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

#lines=`wc -l ./data/weights001_i`
lines=`wc -l $str`
lines=$(echo $lines | cut -d' ' -f 1)

for i in $(seq 1 $lines ); do
  eval ./main $batch $i
  sleep=0.1
done
