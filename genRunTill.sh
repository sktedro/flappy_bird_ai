#!/bin/bash

batch=$1
wantedRecords=$2

if [ -z "$1" ]; then
  echo "genRunTill.sh: Check arguments"
  exit
fi
if [ -z "$2" ]; then
  echo "genRunTill.sh: Check arguments"
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
out="_o"
in="_i"

input=$str1$str2$in
records=$str1$str2$out

weightGenPath="./weightGen/weightGen"

lines=`wc -l $records`
lines=$(echo $lines | cut -d' ' -f 1)

while [ $lines -le $wantedRecords ]; do
  lines=`wc -l $records`
  lines=$(echo $lines | cut -d' ' -f 1)
  eval touch $input
  eval $weightGenPath $batch 100

  #eval sh autorun.sh $batch
  for i in $(seq 1 $lines ); do
    eval ./main $batch $i
  done

  eval rm $input
done
