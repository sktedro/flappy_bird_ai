#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

float ai_birdSpeedW, ai_birdHeightW, ai_canvasHeightW, ai_distanceToBarW, ai_barHeightW;

char filename[] = "./data/weightsXXX_i";

int main(int argc, char **argv){
  if(argc < 3){
    printf("./weightGen [batch] [how many]\n");
    return -1;
  }

  time_t t;
  srand((unsigned) time(&t));

  int batch = atoi(argv[1]);
  int num = atoi(argv[2]);
  float range = 1/(batch);

  int hundreds = batch/100;
  int tens = (batch - 100*hundreds)/10;
  filename[14] = hundreds + '0';
  filename[15] = tens + '0';
  filename[16] = (batch - 100*hundreds - 10*tens) + '0';

  FILE *f;
  f = fopen(filename, "a");
  printf("%s\n", filename);
  if(f == NULL){
    f = fopen(filename, "w");
    if(f == NULL){
      printf("Cannot open the file\n");
      return -1;
    }
    fclose(f);
    f = fopen(filename, "a");
  }

  char *buffer = malloc(100);
  if(batch == 1){
    for(int i = 0; i < num; i++){
      ai_birdSpeedW = (rand()%1000000)/1000000.0;
      ai_birdHeightW = (rand()%1000000)/1000000.0;
      ai_canvasHeightW = (rand()%1000000)/1000000.0;
      ai_distanceToBarW = (rand()%1000000)/1000000.0;
      ai_barHeightW = (rand()%1000000)/1000000.0;

      snprintf(buffer, 20, "%f;", ai_birdSpeedW);
      fputs(buffer, f);
      printf("%s", buffer);
      snprintf(buffer, 20, "%f;", ai_birdHeightW);
      fputs(buffer, f);
      printf("%s", buffer);
      snprintf(buffer, 20, "%f;", ai_canvasHeightW);
      fputs(buffer, f);
      printf("%s", buffer);
      snprintf(buffer, 20, "%f;", ai_distanceToBarW);
      fputs(buffer, f);
      printf("%s", buffer);
      snprintf(buffer, 20, "%f", ai_barHeightW);
      fputs(buffer, f);
      fputc('\n', f);
      printf("%s\n", buffer);
    }
  }
  fclose(f);
  free(buffer);
  //free(fileNameUp);
}
