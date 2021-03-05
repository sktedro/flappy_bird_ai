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



Weight generator:

The weight generation algorithm must be started from it's PARENT folder: ./weightGen/weightGen [batch] [how many]

It generates weights into ../data/weightsXXX_i in format: float;float;float;float;float

For batches higher than 1 it takes data from previous batch (weightsXXX_o).
