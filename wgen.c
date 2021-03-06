#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

FILE *f_i;
FILE *f_o;

float ai_birdSpeedW, ai_birdHeightW, ai_canvasHeightW, ai_distanceToBarW, ai_barHeightW;

char filename_i[] = "./data/weightsXXX_i";
char filename_o[] = "./data/weightsXXX_o";
char delim = ';';

void printWeights(char *buffer){
  snprintf(buffer, 20, "%f;", ai_birdSpeedW);
  fputs(buffer, f_i);
  printf("%s", buffer);
  snprintf(buffer, 20, "%f;", ai_birdHeightW);
  fputs(buffer, f_i);
  printf("%s", buffer);
  snprintf(buffer, 20, "%f;", ai_canvasHeightW);
  fputs(buffer, f_i);
  printf("%s", buffer);
  snprintf(buffer, 20, "%f;", ai_distanceToBarW);
  fputs(buffer, f_i);
  printf("%s", buffer);
  snprintf(buffer, 20, "%f", ai_barHeightW);
  fputs(buffer, f_i);
  fputc('\n', f_i);
  printf("%s\n", buffer);
}

bool getLastWeights(int num){
  /*
  for(int i = 0; i < num - 1; i++){
    char c = ' ';
    while(c != '\n'){
      c = fgetc(f_o);
      if(c == EOF)
        break;
    }
  }
  */

  char *line = malloc(1000);
  for(int i = 0; i < 1000; i++){
    line[i] = fgetc(f_o);
    if(line[i] == '\n' || line[i] == EOF)
      break;
  }

  char *token = strtok(line, &delim);
  if(!token || token[0] == EOF)
    return false;
  strtol(token, NULL, 10);
  token = strtok(NULL, &delim);
  ai_birdSpeedW = strtof(token, NULL);
  token = strtok(NULL, &delim);
  ai_birdHeightW = strtof(token, NULL);
  token = strtok(NULL, &delim);
  ai_canvasHeightW = strtof(token, NULL);
  token = strtok(NULL, &delim);
  ai_distanceToBarW = strtof(token, NULL);
  token = strtok(NULL, &delim);
  ai_barHeightW = strtof(token, NULL);
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
  float range = 1/(batch);
  int mult = atoi(argv[3]);

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
  }

  char *buffer = malloc(100);

  float fluctuation = 1.0; //0.9 to 0.00001 (last batch)
  if(batch != 1)
    fluctuation = 1.0/(3.0*batch) + 1.0/(4.0*batch);

  if(mult == 0)
    mult = pow(batch, 3); //How many new weight sets will be generated using one successful weight set

  for(int i = 0; i < num; i++){
    ai_birdSpeedW = ai_birdHeightW = ai_canvasHeightW = ai_distanceToBarW = ai_barHeightW = 0;
    if(batch != 1)
      getLastWeights(i);
    for(int j = 0; j < mult; j++){
      if(rand()%2 == 0)
        ai_birdSpeedW += (rand()%1000000)/1000000.0*fluctuation;
      else
        ai_birdSpeedW -= (rand()%1000000)/1000000.0*fluctuation;
      if(rand()%2 == 0)
        ai_birdHeightW += (rand()%1000000)/1000000.0*fluctuation;
      else
        ai_birdHeightW -= (rand()%1000000)/1000000.0*fluctuation;
      if(rand()%2 == 0)
        ai_canvasHeightW -= (rand()%1000000)/1000000.0*fluctuation;
      else
        ai_canvasHeightW += (rand()%1000000)/1000000.0*fluctuation;
      if(rand()%2 == 0)
        ai_distanceToBarW += (rand()%1000000)/1000000.0*fluctuation;
      else
        ai_distanceToBarW -= (rand()%1000000)/1000000.0*fluctuation;
      if(rand()%2 == 0)
        ai_barHeightW += (rand()%1000000)/1000000.0*fluctuation;
      else
        ai_barHeightW -= (rand()%1000000)/1000000.0*fluctuation;

      if(ai_birdSpeedW > 1)
        ai_birdSpeedW = 1;
      if(ai_birdSpeedW < 0)
        ai_birdSpeedW = 0;

      if(ai_birdHeightW > 1)
        ai_birdHeightW = 1;
      if(ai_birdHeightW < 0)
        ai_birdHeightW = 0;

      if(ai_canvasHeightW > 1)
        ai_canvasHeightW = 1;
      if(ai_canvasHeightW < 0)
        ai_canvasHeightW = 0;

      if(ai_distanceToBarW > 1)
        ai_distanceToBarW = 1;
      if(ai_distanceToBarW < 0)
        ai_distanceToBarW = 0;

      if(ai_barHeightW > 1)
        ai_barHeightW = 1;
      if(ai_barHeightW < 0)
        ai_barHeightW = 0;

      printWeights(buffer);
    }
  }
  fclose(f_i);
  if(batch != 1)
    fclose(f_o);
  free(buffer);
}
