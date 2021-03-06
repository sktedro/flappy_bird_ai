#!/bin/bash

if [ -z "$1" ] || [ -z "$2" ] || [ -z "$3" ]; then
  echo "genRunTill.sh: Check arguments"
  exit
fi

batch=$1
birds=$2
wantedRecords=$3

buildFolder="./build/"
wgenPath=$buildFolder"wgen"
sortPath=$buildFolder"sort"
gamePath=$buildFolder"game"

echo Welcome, I am Tedro\'s flappy bird AI!
echo First, I\'ll compile the files for you:
echo
eval make
echo
echo Good. Now\'s the time for me to practice
echo
echo Starting with batch $batch, I will play with $birds birds 
echo until I have $wantedRecords successful outputs. 
echo Then I will go on to the next batch automatically. 
echo You just sit tight.
echo

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

  run=0

  #WHILE WE WANT MORE OUTPUT WEIGHT SETS THAN WE ALREADY HAVE
  while [ $recordLines -lt $wantedRecords ]; do
    run=$((run+1))

    #GENERATE INPUT WEIGHTS AND COUNT THEM
    eval rm -f $inputPath
    eval touch $inputPath
    eval $wgenPath $batch $birds 1

    #RUN THE GAME FOR EVERY INPUT WEIGHT
    echo Now have $recordLines out of $wantedRecords output weight sets
    echo Running batch $batch, run $run.
    eval $gamePath $batch $birds

    #COUNT OUTPUT WEIGHT SETS EVERY ITERATION
    recordLines=`wc -l $recordsPath | cut -d' ' -f 1`
  done

  #SORT THE OUTPUT
  eval $sortPath $recordsPath

  echo Batch successfully finished with $recordLines records

  IFS=';' read -r bestScore b < $recordsPath
  echo The most successful bird of batch $batch flew through $bestScore barriers!

  batch=$((batch+1))
  echo
  echo ===== Continuing with batch $batch =====
  echo
done
