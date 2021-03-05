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

inputPath=$str1$str2$in
recordsPath=$str1$str2$out

weightGenPath="./wgen/wgen"

#eval touch $recordsPath
#eval touch $inputPath

recordLines=`wc -l $recordsPath`
recordLines=$(echo $recordLines | cut -d' ' -f 1)

while [ $recordLines -lt $wantedRecords ]; do
  eval touch $inputPath
  eval $weightGenPath $batch 100


  inputLines=`wc -l $inputPath`
  inputLines=$(echo $inputLines | cut -d' ' -f 1)

  for i in $(seq 1 $inputLines ); do
    eval ./main $batch $i
  done

  eval rm -f $inputPath

  recordLines=`wc -l $recordsPath`
  recordLines=$(echo $recordLines | cut -d' ' -f 1)
  if [ $recordLines -ge $wantedRecords ]; then
    exit
  fi
done
