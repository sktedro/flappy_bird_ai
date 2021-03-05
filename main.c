#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
#include <math.h>


#define ai 1 //Toggle AI instead of the user playing the game
#define aiDebug 0 //Will print AI weights
#define background 1 //Toggle running on background (only data, no "images")

#define canvasx 100 //Canvas width
#define canvasy 35 //Canvas height

#define speed 100 //Flight speed (x direction) - the higher the slower

#define jumpHeight 2.0

#define birdx 10 //Bird location from the left
#define birdPixels 14 //Don't mind this

#define barWidth 8 //Width of a single barrier
#define barGap 10 //Gap between top and bottom barriers


float ai_birdSpeed, ai_birdHeight, ai_distanceToBar;
int ai_canvasHeight, ai_barHeight;
float ai_birdSpeedW, ai_birdHeightW, ai_canvasHeightW, ai_distanceToBarW, ai_barHeightW;

char filename[] = "./data/weightsXXX_i";

struct timeval lastCheck, actTime;

typedef struct{
  float y;
  float fall_speed;
  int **pixels;
} Bird;

typedef struct{
  int x1;
  int x2;
  int lowy;
  int highy;
} Barrier;


/*
 * Barrier functions
 */

void newBar(Barrier *bar){
  ai_barHeight = rand()%(canvasy - 2*barGap) + barGap;
  bar->highy = ai_barHeight + barGap/2;
  bar->lowy = ai_barHeight - barGap/2;
  bar->x1 = bar->x1 = canvasx - 1;
  bar->x2 = bar->x2 = canvasx + barWidth;
}

void updateBar(Barrier *bar){
  (bar->x1)--;
  (bar->x2)--;
  (bar->x1)--;
  (bar->x2)--;
}

void printBarrier(char canvas[canvasx][canvasy], Barrier bar){
  for(int i = 0; i < bar.lowy; i++){
    if(bar.x1 >= 0)
      canvas[bar.x1][i] = '#';
    if(bar.x2 >= 0 && bar.x2 < canvasx)
      canvas[bar.x2][i] = '#';
  }
  for(int i = bar.highy; i < canvasy; i++){
    if(bar.x1 >= 0)
      canvas[bar.x1][i] = '#';
    if(bar.x2 >= 0 && bar.x2 < canvasx)
      canvas[bar.x2][i] = '#';
  }
  for(int i = bar.x1; i <= bar.x2; i++){
    if(i < 0 || i >= canvasx)
      break;
    canvas[i][bar.lowy] = '#';
    canvas[i][bar.highy] = '#';
  }
}

/*
 * Bird functions
 */ 

bool birdInit(Bird *bird){
  bird->y = (int)canvasy/2;
  bird->fall_speed = 0;
  bird->pixels = malloc(birdPixels*sizeof(void*));
  if(!bird->pixels)
    return false;
  for(int i = 0; i < birdPixels; i++){
    bird->pixels[i] = malloc(2*sizeof(int));
    if(!bird->pixels[i])
      return false;
  }
  bird->pixels[0][0] = birdx+3;
  bird->pixels[1][0] = birdx-3;
  bird->pixels[2][0] = bird->pixels[4][0] = bird->pixels[8][0] = birdx+2;
  bird->pixels[3][0] = bird->pixels[5][0] = bird->pixels[9][0] = birdx-2;
  bird->pixels[6][0] = bird->pixels[10][0] = birdx+1;
  bird->pixels[7][0] = bird->pixels[11][0] = birdx-1;
  bird->pixels[12][0] = bird->pixels[13][0] = birdx;
  return true;
}

void printBird(char canvas[canvasx][canvasy], Bird bird){
  if(bird.y >= canvasy && bird.y < 0)
    return;
  int pixels[birdPixels][2] = {
    {birdx+3, bird.y},
    {birdx-3, bird.y},
    {birdx+2, bird.y},
    {birdx-2, bird.y},
    {birdx+2, bird.y+1},
    {birdx-2, bird.y+1},
    {birdx+2, bird.y-1},
    {birdx-2, bird.y-1},
    {birdx+1, bird.y+1},
    {birdx-1, bird.y+1},
    {birdx+1, bird.y-1},
    {birdx-1, bird.y-1},
    {birdx, bird.y+1},
    {birdx, bird.y-1}
  };
  for(int i = 0; i < birdPixels; i++){
    for(int j = 0; j < 2; j++){
      canvas[bird.pixels[i][0]][bird.pixels[i][1]] = '%';
    }
  }
}

bool checkBird(Bird bird, Barrier *bar, int *score){
  if(bar->x2 < 0){
    newBar(bar);
    (*score)++;
  }
  if(bird.y < 0 || bird.y > canvasy){
    printf("You lost. Score: %d\n", *score);
    return false;
  }
  for(int i = 0; i < birdPixels; i++){
    for(int j = 0; j < 2; j++){
      if(bird.pixels[i][0] >= bar->x1 && bird.pixels[i][0] <= bar->x2){
        if(bird.pixels[i][1] <= bar->lowy || bird.pixels[i][1] >= bar->highy){
          printf("You lost. Score: %d\n", *score);
          return false;
        }
      }
    }
  }
  return true;
}

void updateBird(Bird *bird){
  bird->pixels[0][1] = bird->pixels[1][1] = bird->pixels[2][1] = bird->pixels[3][1] =
    bird->y;
  bird->pixels[4][1] = bird->pixels[5][1] = bird->pixels[6][1] = bird->pixels[7][1] =
    bird->pixels[12][1] = bird->y+1;
  bird->pixels[8][1] = bird->pixels[9][1] = bird->pixels[10][1]= bird->pixels[11][1]=
    bird->pixels[13][1] = bird->y-1;
}

void freeBirdPixels(Bird bird){
  for(int i = 0; i < birdPixels; i++)
    free(bird.pixels[i]);
  free(bird.pixels);
}

/*
 * Other game functions
 */

void printCanvas(char canvas[canvasx][canvasy], Bird bird, Barrier bar){
  for(int i = 0; i < canvasx; i++)
    for(int j = 0; j < canvasy; j++)
      canvas[i][j] = ' ';
  printBird(canvas, bird);
  printBarrier(canvas, bar);

  system("clear");
  for(int y = 0; y < canvasy; y++){
    for(int x = 0; x < canvasx; x++){
      printf("%c", canvas[x][y]);
    }
    printf("\n");
  }
}

bool jump(){
  initscr();
  timeout(1);
  if(getch() != ERR){
    endwin();
    return true;
  }
  endwin();
  return false;
}

/*
 * AI functions
 */

bool ai_jump(){
  float decision =
    ai_birdSpeed * ai_birdSpeedW +
    ai_birdHeight * ai_birdHeightW/10 +
    ai_canvasHeight * ai_canvasHeightW/50 +
    ai_distanceToBar * ai_distanceToBarW/20 +
    ai_barHeight * ai_barHeightW/10;
  if(decision > 0.5 * 5)
    return true;
  return false;
}

bool ai_getWeights(int batch, int setting){
  FILE *f;
  char *buffer;
  size_t bufferSize;

  int hundreds = batch/100;
  int tens = (batch - 100*hundreds)/10;
  filename[14] = hundreds + '0';
  filename[15] = tens + '0';
  filename[16] = (batch - 100*hundreds - 10*tens) + '0';

  f = fopen(filename, "r");
  if(!f){
    printf("Error locating %s.\n", filename);
    return false;
  }

  //Get to the desired line
  for(int i = 0; i < setting - 1; i++){
    char c = ' ';
    while(c != '\n'){
      c = fgetc(f);
      if(c == EOF)
        break;
    }
  }

  //Get one line
  char *line = malloc(1000);
  for(int i = 0; i < 1000; i++){
    line[i] = fgetc(f);
    if(line[i] == '\n' || line[i] == EOF)
      break;
  }
  //printf("%s", line);

  //Get floats from that line
  char delim = ';';
  char *token = strtok(line, &delim);
  if(!token || token[0] == EOF)
    return false;
  ai_birdSpeedW = strtof(token, NULL);
  token = strtok(NULL, &delim);
  ai_birdHeightW = strtof(token, NULL);
  token = strtok(NULL, &delim);
  ai_canvasHeightW = strtof(token, NULL);
  token = strtok(NULL, &delim);
  ai_distanceToBarW = strtof(token, NULL);
  token = strtok(NULL, &delim);
  ai_barHeightW = strtof(token, NULL);
  //printf("birdSpeedW = %g\nbirdHeightW = %g\ncanvasHeightW = %g\n distanceToBarW = %g\nbarHeightW = %g\n", ai_birdSpeedW, ai_birdHeightW, ai_canvasHeightW, ai_distanceToBarW, ai_barHeightW);

  free(line);
  fclose(f);
  return true;
}

void ai_printWeights(int score){
  filename[18] = 'o';
  FILE *f = fopen(filename, "a");
  char *buffer = malloc(100);
  snprintf(buffer, 20, "%d;", score);
  fputs(buffer, f);
  snprintf(buffer, 20, "%f;", ai_birdSpeedW);
  fputs(buffer, f);
  snprintf(buffer, 20, "%f;", ai_birdHeightW);
  fputs(buffer, f);
  snprintf(buffer, 20, "%f;", ai_canvasHeightW);
  fputs(buffer, f);
  snprintf(buffer, 20, "%f;", ai_distanceToBarW);
  fputs(buffer, f);
  snprintf(buffer, 20, "%f", ai_barHeightW);
  fputs(buffer, f);
  fputc('\n', f);
  fclose(f);
  free(buffer);
}

/*
 *
 * MAIN
 *
 */

int main(int argc, char **argv){
  //AI initialisation
  int batch, setting;
  if(ai){
    if(argc < 3){
      printf("main.c: Too few arguments\n");
      return -1;
    }
    ai_canvasHeight = canvasy;
    batch = atoi(argv[1]);
    setting = atoi(argv[2]);
    if(!ai_getWeights(batch, setting))
      return -1;
    if(aiDebug){
      printf("File: %s\n", filename);
      printf("birdSpeedW = %g\nbirdHeightW = %g\ncanvasHeightW = %g\ndistanceToBarW = %g\nbarHeightW = %g\n", 
          ai_birdSpeedW, ai_birdHeightW, ai_canvasHeightW, ai_distanceToBarW, ai_barHeightW);
      return 0;
    }
  }

  //Init of a canvas, a bird and a barrier
  char canvas[canvasx][canvasy];
  Bird bird;
  if(!birdInit(&bird))
    return -1;
  Barrier bar;

  //Time init (for rand(), time difference calculation etc.)
  time_t t = 0;
  srand((unsigned) time(&t));
  gettimeofday(&lastCheck, NULL);
  gettimeofday(&actTime, NULL);
  u_int64_t time = actTime.tv_usec + actTime.tv_sec;
  u_int64_t nextUpdate = 0, timediff = 0;

  int score = -1;
  bool gameOver = false;

  while(1){
    //Check, if the command to jump has been given
    if((!ai && jump()) || (ai && ai_jump()))
      bird.fall_speed = -jumpHeight;

    //Check, if bird isn't dead
    if(!checkBird(bird, &bar, &score))
      break;

    //Only update the screen "once in a while"
    gettimeofday(&actTime, NULL);
    u_int64_t time = actTime.tv_usec + actTime.tv_sec*1000000;
    if(time > nextUpdate){
      nextUpdate = time + 400*speed;

      //Calculate next position of the bird while keeping gravity in mind
      timediff = (u_int64_t)actTime.tv_sec - (u_int64_t)lastCheck.tv_sec;
      gettimeofday(&lastCheck, NULL);
      bird.fall_speed += timediff/4.0 + 0.3;
      bird.y = bird.y + bird.fall_speed;

      //Update variables for AI
      if(ai){
        ai_birdSpeed = bird.fall_speed;
        ai_birdHeight = bird.y;
        ai_distanceToBar = (bar.x1 + bar.x2)/2 - birdx;
      }

      //Update canvas in memory
      updateBird(&bird);
      updateBar(&bar);

      //Print updated canvas
      if(!background){
        printCanvas(canvas, bird, bar);
      }

      //Print stats
      if(!background){
        printf("Score = %d\n", score);
        printf("Canvas Height: %d\nSpeed: %g\nHeight: %g\nDistance to next bar: %g\nNext bar height: %d\n",
            ai_canvasHeight, ai_birdSpeed, ai_birdHeight, ai_distanceToBar, ai_barHeight);
      }
    }
  }

  //Save the weights that lead to successful game (score>0)
  if(ai && score > 0)
    ai_printWeights(score);

  freeBirdPixels(bird);
  return 0;
}
