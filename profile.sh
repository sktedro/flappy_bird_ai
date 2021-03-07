#!/bin/bash

echo Welcome. 
echo I will run profiling on \'wgen\' with 10\'000 desired sets for batch 1 and batch 2 with only one line from batch 1
echo Then I will run profiling on \'game\' with 1, 100 and 10\'000 birds
eval make clean
eval make profile

eval mkdir -p profiles

#WGEN profiling

touch ./data/weights001_i
eval ./build/wgen 1 10000 1
gprof ./build/wgen gmon.out > profiles/profile_wgen_batch1.out

touch ./data/weights002_i
echo "1|0.500000|0.500000|0.500000|0.500000" > ./data/weights001_o
eval ./build/wgen 2 10000 1
gprof ./build/wgen gmon.out > profiles/profile_wgen_batch2.out

echo Weight sets generator profiling done

#GAME profiling
echo "1|0.500000|0.500000|0.500000|0.500000" > ./data/weights001_i
eval ./build/game 1 1
gprof ./build/game gmon.out > profiles/profile_game_1.out

eval ./build/game 1 100
gprof ./build/game gmon.out > profiles/profile_game_100.out

eval ./build/game 1 10000
gprof ./build/game gmon.out > profiles/profile_game_10000.out

echo Game profiling done

rm -f gmon.out

eval make
echo Thank you, profiling ouputs are saved to \'./profiles/\'.
