#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ]; then
  echo "genRunTill.sh: Check arguments"
  exit
fi

batch=$1
wantedRecords=$2

buildFolder="./build/"
wgenPath=$buildFolder"wgen"
sortPath=$buildFolder"sort"
gamePath=$buildFolder"game"

eval make

while true; do
  #GENERATE INPUT AND OUTPUT FILE NAMES
  wpath="./data/weights"
  if [ $batch -le 9 ]; then
      wbatch="00"$batch
  elif [ $batch -ge 10 ]; then
      wbatch="0"$batch
  elif [ $batch -ge 100 ]; then
      wbatch=$batch
  fi
  wout="_o"
  win="_i"
  inputPath=$wpath$wbatch$win
  recordsPath=$wpath$wbatch$wout

  #CREATE INPUT AND OUTPUT FILES IF THEY DON'T EXIST
  eval touch $recordsPath
  eval touch $inputPath

  #COUNT HOW MANY OUTPUT WEIGHT SETS THERE ALREADY ARE
  recordLines=`wc -l $recordsPath | cut -d' ' -f 1`

  #WHILE WE WANT MORE OUTPUT WEIGHT SETS THAN WE ALREADY HAVE
  while [ $recordLines -lt $wantedRecords ]; do
    #GENERATE INPUT WEIGHTS AND COUNT THEM
    eval rm -f $inputPath
    eval touch $inputPath
    eval $wgenPath $batch 100 1
    inputLines=`wc -l $inputPath | cut -d' ' -f 1`

    #RUN THE GAME FOR EVERY INPUT WEIGHT
    for i in $(seq 1 $inputLines ); do
      echo Running batch $batch, line $i of $inputLines lines in $inputPath:
      eval $gamePath $batch $i
    done

    #COUNT OUTPUT WEIGHT SETS EVERY ITERATION
    recordLines=`wc -l $recordsPath | cut -d' ' -f 1`
  done
  eval $sortPath $recordsPath
  batch=$((batch+1))
done
