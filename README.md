Project name: Cheap flappy bird AI
Author: Tedro

Goal is to create a flappy bird game playable in terminal and then implement an
AI to teach itself to play the game.



To run AI learning:
  ./main [batchNumber] [weightsLine]
  while batchNumber is the learning itearion count = first is 1 and so on...
  and weightsLine is a number of the line that the weights will be read from

To just play the game:
  ./main

The input data should be in ./data/ folder and under the name
"weightsXXX_i", while XXX is the batch number (for which batch the inputs
are intended).
The data should be in floats (0.0 to 1.0) and separated by ;
Each line consists of (actually) 5 weights:
ai_birdSpeed, ai_birdHeight, ai_canvasHeight, ai_distanceToBar and ai_barHeight

Outputs from the batch will be in the same folder and under the name
"weightsXXX_o". Only successful outputs will be written here (for the
first batch, the ones that cross the first barrier) and each line will be:
score, ai_birdSpeed, ai_birdHeight, ai_canvasHeight, ai_distanceToBar and ai_barHeight
while "score" is the score that the AI achieved with those weights.




makefile
  - Builds main.c, sort.c and wgen.c in build/ folder upon "make" command

./automake
  - Automatically build when something changes
  Run on the background so the project will be built every time anything in the
  directory changes

sh autorun.sh [batch] [desired amount of outputs] [amount of birds per execution]
  - AI Trainer
  Automatically generates weights based on previous batch (or totally randomly
  if batch number is 0), tests these weights in the game and every weight that
  was successful (more than one barrier was crossed by the bird) is then
  written to weightsXXX_o file in data/
  TODO: Automatically sort when done
  TODO: Automatically run next batch when done
  TODO: If no batch number entered, go from the last one

./build/main [] []

./build/sort [] []
  - AI Output sorter - sorts weight sets based on score that was achieved using that set

./build/wgen [batch] [amount of weights from previous batch to use] [multiplier]
  - AI Weight generator
  It generates weights into ../data/weightsXXX_i in format: float;float;float;float;float
  For batches higher than 1 it takes data from previous batch (weightsXXX_o)
  and randomly adds or substracts fractions of 1.
  If multiplier is zero, it will generate exponentially more weight sets with
  the batch number rising. If multiplier is 1, for one output, one input will
  be generated and so on.
  './build/wgen 3 100 0' will take first 100 lines of ../data/weights002_o and
  for each line generate 100*(3^2) weight sets in ../data/weights003_i
  
