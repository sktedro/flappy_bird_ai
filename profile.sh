#!/bin/bash

echo Welcome. I will run profiling on \'game\' with 1, 100 and 10\'000 birds
eval mkdir -p profiles
eval make profile
eval ./build/game 1 1
gprof ./build/game gmon.out > profiles/profile_1.out
eval ./build/game 1 100
gprof ./build/game gmon.out > profiles/profile_100.out
eval ./build/game 1 10000
gprof ./build/game gmon.out > profiles/profile_10000.out
rm -f gmon.out
eval make
echo Thank you, profiling ouput is saved to \'./profiles/profile_$.out\'.
