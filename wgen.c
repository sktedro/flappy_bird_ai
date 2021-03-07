#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "settings.h"

FILE *f_i;
FILE *f_o;

char filename_i[] = "./data/weightsXXX_i";
char filename_o[] = "./data/weightsXXX_o";
char delim = '|';

int countLines(){
  fseek(f_o, 0, SEEK_SET);
  int count = -1;
  char c = 'a';
  while(1){
    c = fgetc(f_o);
    if(c == '\n')
      count++;
    if(c == EOF)
      break;
  }
  fseek(f_o, 0, SEEK_SET);
  return count;
}

void printWeights(char *buffer, float stats[weightsCount]){
  for(int i = 0; i < weightsCount; i++){
    if(i == weightsCount - 1)
      snprintf(buffer, 20, "%f\n", stats[i]);
    else
      snprintf(buffer, 20, "%f%c", stats[i], delim);
    if(wgen_printData)
      printf("%s", buffer);
    fputs(buffer, f_i);
  }
}

void getLastWeights(int num, char *line, char *token, float stats[weightsCount]){
  //Load the desired line
  for(int i = 0; i < 999; i++){
    line[i] = fgetc(f_o);
    if(line[i] == '\n' || line[i] == EOF){
      line[i] = '\0';
      line[i+1] = '\n';
      break;
    }
  }
  token = strtok(line, &delim); //Skip score data
  //Save the weights
  for(int i = 0; i < weightsCount; i++){
    token = strtok(NULL, &delim);
    stats[i] = strtof(token, NULL);
  }
}

int main(int argc, char **argv){
  if(argc < 4){
    printf("./weightGen [batch] [how many] [multiplier]\n");
    return -1;
  }

  time_t t;
  srand((unsigned) time(&t));

  int batch = atoi(argv[1]);
  int lastBatch = batch - 1;
  int num = atoi(argv[2]);
  int mult = atoi(argv[3]);
  int linesToGenFrom;

  int hundreds = batch/100;
  int tens = (batch - 100*hundreds)/10;
  filename_i[14] = hundreds + '0';
  filename_i[15] = tens + '0';
  filename_i[16] = (batch - 100*hundreds - 10*tens) + '0';

  hundreds = lastBatch/100;
  tens = (lastBatch - 100*hundreds)/10;
  filename_o[14] = hundreds + '0';
  filename_o[15] = tens + '0';
  filename_o[16] = (lastBatch - 100*hundreds - 10*tens) + '0';

  f_i = fopen(filename_i, "a");
  if(wgen_printData)
    printf("%s\n", filename_i);
  if(f_i == NULL){
    f_i = fopen(filename_i, "w");
    if(f_i == NULL){
      printf("Cannot open the file\n");
      return -1;
    }
    fclose(f_i);
    f_i = fopen(filename_i, "a");
  }

  if(batch != 1){
    f_o = fopen(filename_o, "r");
    if(!f_o){
      printf("wgen.c: Error locating %s.\n", filename_o);
      return -1;
    }
    linesToGenFrom = countLines();
  }

  char *buffer = malloc(100);

  float fluctuation = 1.0; //0.9 to 0.00001 (last batch)
  if(batch != 1)
    fluctuation = -((log(batch)) / 7.0 + 1.0);
    //fluctuation = 1.0/(3.0*batch) + 1.0/(4.0*batch);

  if(mult == 0)
    mult = pow(batch, 3); //How many new weight sets will be generated using one successful weight set

  char *line = malloc(1000);
  char *token = 0;
  float stats[weightsCount];

  for(int i = 0; i < num; i++){
    for(int j = 0; j < weightsCount; j++)
      stats[j] = 0;
    if(batch != 1){

      //If we're at the end of the file to generate from, go to the first line
      if(i >= linesToGenFrom)
        fseek(f_o, 0, SEEK_SET);

      getLastWeights(i, line, token, stats);

    }
    for(int j = 0; j < mult; j++){
      for(int k = 0; k < weightsCount; k++){
        if(rand()%2 == 0 || batch == 1)
          stats[k] += (rand()%1000000)/1000000.0*fluctuation;
        else
          stats[k] -= (rand()%1000000)/1000000.0*fluctuation;
        if(stats[k] > 1)
          stats[k] = 1;
        else if(stats[k] < 0)
          stats[k] = 0;
      }
      printWeights(buffer, stats);
    }
  }

  free(line);
  fclose(f_i);
  if(batch != 1)
    fclose(f_o);
  free(buffer);
}
