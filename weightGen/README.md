The weight generation algorithm must be started from it's folder:
./weightGen [batch] [how many]

It generates weights into ../data/weightsXXX_i in format:
float;float;float;float;float

For batches higher than 1 it takes data from previous batch (weightsXXX_o).
