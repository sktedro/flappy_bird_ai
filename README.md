Project name: Cheap flappy bird AI

Author: Tedro

Goal is to create a flappy bird game playable in terminal and then implement an
AI to teach itself to play the game for eternity.

To just play the game, type 'make' and './build/game'.

To run the AI, run for example 'sh run.sh 1 1 1', which will run AI learning
starting with batch one, with one bird per game and while requiring at least
one bird achieving score higher or equal to the batch number to continue to next barch.

For more detailed explaination, read instructions below.

This is really overengineered, because of a simple mistake that the bar gap was
too thin and bird's jump was too high, so it was impossible to cross 'many'
barriers. Of course, I did not realise that until I implemented every possible
thing that came to my mind that could improve the rate at which the AI
learns... 

FILES AND INSTRUCTIONS:

The AI input data will be in ./data/ folder and under the name
"weightsXXX_i", while XXX is the batch number (for which batch the inputs
are intended).
The data should be in floats (-1.0 to 1.0) and separated by '|'
Each line consists of (actually) 3 weights:
bird's fall speed|Y difference to the next bar|bias (always set to 1)

Outputs from the batch will be in the same folder and under the name
"weightsXXX_o". Only successful outputs will be written here (for the
first batch, the ones that cross the first barrier) and each line will be
the same as the input data, but with score at the beginning of the line.
"score" is the amount of barriers that the AI flew through with those weights.


./build/game
  - To just play the game, enter no arguments 
  - To run AI learning manually (not recommended): 
    './build/game [batch number] [number of birds in one game]'
    while batchNumber is the learning iteration = first is 1 and so on...

make
  - Calls makefile and builds main.c, sort.c and wgen.c in build/

./automake
  - Automatically build when something changes
  - Run on the background so the project will be built every time anything in the
  directory changes

sh autorun.sh [batch] [amount of input lines to test] [desired amount of outputs]
  - AI Trainer
  - Automatically generates weights based on previous batch (or totally randomly
  if batch number is 1), tests these weights in the game and every weight that
  was successful (more than one barrier was crossed by the bird) is then
  written to weightsXXX_o file in data/
  - The amount of input lines to test is also the amount of birds that will be
  generated and "played" in one game.
  - When enough output weight sets are generated, the output is automatically sorted 
  based on score and runs next batch. This repeats until killed

./build/wgen [batch] [amount of weights from previous batch to use] [multiplier]
  - AI Weight generator
  - It generates weights (ranging from -1.0 to 1.0) into ../data/weightsXXX_i 
  - For batches higher than 1 it takes data from previous batch (weightsXXX_o)
  and randomly adds or substracts fractions of 1 - see wgen.c for the formula.
  - If multiplier is zero, it will generate exponentially more weight sets with
  the batch number rising. If multiplier is 1, for one output, one input will
  be generated and so on.
  - For example: './build/wgen 3 100 0' will take first 100 lines of ../data/weights002_o 
  and for each line generate 100*(3^2) weight sets in ../data/weights003_i
  
./build/sort [path to file]
  - AI Output sorter - sorts weight sets based on score that was achieved using that set
