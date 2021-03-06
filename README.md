Project name: Cheap flappy bird AI

Author: Tedro

Goal is to create a flappy bird game playable in terminal and then implement an
AI to teach itself to play the game for eternity.

To run the AI, run for example 'sh run.sh 1 100 1000'
For more detailed explaination, read below:

FILES AND INSTRUCTIONS:

The AI input data should be in ./data/ folder and under the name
"weightsXXX_i", while XXX is the batch number (for which batch the inputs
are intended).
The data should be in floats (0.0 to 1.0) and separated by ;
Each line consists of (actually) 5 weights:
ai_birdSpeed;ai_birdHeight;ai_canvasHeight;ai_distanceToBar;ai_barHeight

Outputs from the batch will be in the same folder and under the name
"weightsXXX_o". Only successful outputs will be written here (for the
first batch, the ones that cross the first barrier) and each line will be:
score;ai_birdSpeed;ai_birdHeight;ai_canvasHeight;ai_distanceToBar;ai_barHeight
while "score" is the score that the AI achieved with those weights.


./build/game
  - To just play the game, enter no arguments 
  - To run AI learning manually (not recommended): 
    './build/game [batch number] [number of birds in one game]
    while batchNumber is the learning iteration = first is 1 and so on...

make
  - Calls makefile and builds main.c, sort.c and wgen.c in build/

./automake
  - Automatically build when something changes
  Run on the background so the project will be built every time anything in the
  directory changes

sh autorun.sh [batch] [amount of input lines to test] [desired amount of outputs]
  - AI Trainer
  Automatically generates weights based on previous batch (or totally randomly
  if batch number is 1), tests these weights in the game and every weight that
  was successful (more than one barrier was crossed by the bird) is then
  written to weightsXXX_o file in data/
  The amount of input lines to test is also the amount of birds that will be
  generated and "played" in one game.
  When enough output weight sets are generated, the output is automatically sorted 
  based on score and runs next batch. This repeats until killed
  TODO: If no batch number entered, go from the last one

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
  
./build/sort [path to file]
  - AI Output sorter - sorts weight sets based on score that was achieved using that set
