#!/bin/bash

batch=$1
wantedRecords=$2

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
out="_o"
in="_i"

input=$str1$str2$in
records=$str1$str2$out

weightGenPath="./weightGen/weightGen"

lines=`wc -l $records`
lines=$(echo $lines | cut -d' ' -f 1)

echo $lines $wantedRecords
while [ $lines -le $wantedRecords ]; do
  eval touch $input
  sleep=0.1
  eval $weightGenPath $batch 100
  sleep=0.1
  eval sh autorun.sh $batch
  sleep=0.1
  eval rm $input
  sleep=0.1
done
