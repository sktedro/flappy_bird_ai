#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
#include <math.h>

#include "settings.h"

#define aiDebug 0 //Will print AI weights

#define speed 100 //Flight speed (x direction) - the higher the slower
#define jumpHeight 1.2

#define canvasx 100 //Canvas width
#define canvasy 35 //Canvas height

#define birdx 10 //Bird center location from the left
#define birdPixels 14 //Don't mind this
#define barWidth 8 //Width of a single barrier
#define barGap 10 //Gap between top and bottom barriers

bool ai = true; //Is the AI or the user playing?

char filename[] = "./data/weightsXXX_i";
char delim = '|';

struct timeval lastCheck, actTime;

typedef struct{
  bool alive;
  int score;
  float y;
  float fallSpeed;
  float xToNextBar;
  float yToNextBar;
  int **pixels;
  float stats[weightsCount];
  float ai_stats[weightsCount]; //stats converted to AI friendly numbers (range 0-1)
  float weights[weightsCount];
} Bird;

typedef struct{
  int height;
  int x1;
  int x2;
  int lowy;
  int highy;
} Barrier;

/*
 * Edit this if you want to change the decision-making
 */

enum birdStats{yI, fallSpeedI, xToNextBarI, yToNextBarI};

void ai_generateAiStats(Bird *bird){
  bird->ai_stats[yI] = (bird->stats[yI])/(canvasy - barGap);
  bird->ai_stats[fallSpeedI] = bird->stats[fallSpeedI];
  bird->ai_stats[xToNextBarI] = 1/(bird->stats[xToNextBarI]); 
  bird->ai_stats[yToNextBarI] = (2*(bird->stats[yToNextBarI]))/canvasy;
}

void updateBird(Bird *birds, Barrier bar, int i, u_int64_t timediff){
  birds[i].fallSpeed += timediff/2.0 + 0.1;
  birds[i].y = birds[i].y + birds[i].fallSpeed;
  birds[i].stats[0] = birds[i].y;
  birds[i].stats[1] = birds[i].fallSpeed;
  birds[i].stats[2] = birds[i].xToNextBar = (bar.x1 + bar.x2)/2 - birdx;
  birds[i].stats[3] = birds[i].yToNextBar = birds[i].y - bar.height;

  birds[i].pixels[0][1] = birds[i].pixels[1][1] = birds[i].pixels[2][1] = birds[i].pixels[3][1] =
    birds[i].y;
  birds[i].pixels[4][1] = birds[i].pixels[5][1] = birds[i].pixels[6][1] = birds[i].pixels[7][1] =
    birds[i].pixels[12][1] = birds[i].y+1;
  birds[i].pixels[8][1] = birds[i].pixels[9][1] = birds[i].pixels[10][1]= birds[i].pixels[11][1]=
    birds[i].pixels[13][1] = birds[i].y-1;
}

/*
 * Barrier functions
 */

void newBar(Barrier *bar, int highestScore){
  if(firstBars && highestScore <= 1){ //if... then the first three bars will be predefined
    if(highestScore == -1)
      bar->height = barGap;
    else if(highestScore == 0)
      bar->height = canvasy - barGap - 1;
    else if(highestScore == 1)
      bar->height = canvasy/2;
  }else
    bar->height = rand()%(canvasy - 2*barGap) + barGap;
  bar->highy = bar->height + barGap/2;
  bar->lowy = bar->height - barGap/2;
  bar->x1 = canvasx - 1;
  bar->x2 = canvasx + barWidth;
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

bool birdInit(Bird *bird, int birdsCount){
  for(int i = 0; i < birdsCount; i++){
    bird[i].alive = true;
    bird[i].score = -1;
    bird[i].y = canvasy/2;
    bird[i].fallSpeed = 0;
    bird[i].pixels = malloc(birdPixels*sizeof(void*));
    if(!bird[i].pixels)
      return false;
    for(int j = 0; j < birdPixels; j++){
      bird[i].pixels[j] = malloc(2*sizeof(int));
      if(!bird[i].pixels[j])
        return false;
    }
    bird[i].pixels[0][0] = birdx+3;
    bird[i].pixels[1][0] = birdx-3;
    bird[i].pixels[2][0] = bird[i].pixels[4][0] = bird[i].pixels[8][0] = birdx+2;
    bird[i].pixels[3][0] = bird[i].pixels[5][0] = bird[i].pixels[9][0] = birdx-2;
    bird[i].pixels[6][0] = bird[i].pixels[10][0] = birdx+1;
    bird[i].pixels[7][0] = bird[i].pixels[11][0] = birdx-1;
    bird[i].pixels[12][0] = bird[i].pixels[13][0] = birdx;
  }
  return true;
}

void printBird(char canvas[canvasx][canvasy], Bird bird){
  if(bird.y >= canvasy && bird.y < 0)
    return;
  for(int i = 0; i < birdPixels; i++){
    for(int j = 0; j < 2; j++){
      canvas[bird.pixels[i][0]][bird.pixels[i][1]] = '%';
    }
  }
}

void checkBar(Barrier *bar, Bird *birds, int birdsCount, int *highestScore){
  if(bar->x2 < 0){
    newBar(bar, *highestScore);
    for(int i = 0; i < birdsCount; i++){
      if(birds[i].alive){
        (birds[i].score)++;
        if(birds[i].score > *highestScore)
          *highestScore = birds[i].score;
      }
    }
  }
}

bool checkBird(Bird *bird, Barrier *bar){
  if(bird->y < 0 || bird->y >= canvasy)
    return false;
  if(bird->pixels[0][0] < bar->x1 || bird->pixels[1][0] > bar->x2) //Just for optimalisation
    return true;
  for(int i = 0; i < birdPixels; i++){
    for(int j = 0; j < 2; j++){
      if(bird->pixels[i][0] >= bar->x1 && bird->pixels[i][0] <= bar->x2){
        if(bird->pixels[i][1] <= bar->lowy || bird->pixels[i][1] >= bar->highy){
          return false;
        }
      }
    }
  }
  return true;
}

void freeBirds(Bird *birds, int birdsCount){
  for(int i = 0; i < birdsCount; i++){
    for(int j = 0; j < birdPixels; j++)
      free(birds[i].pixels[j]);
    free(birds[i].pixels);
  }
  free(birds);
}

/*
 * Other game functions
 */

void printOut(char canvas[canvasx][canvasy], Bird *bird, int birdsCount, Barrier bar, int highestScore){
  for(int i = 0; i < canvasx; i++)
    for(int j = 0; j < canvasy; j++)
      canvas[i][j] = ' ';
  for(int i = 0; i < birdsCount; i++){
    if(bird[i].alive)
      printBird(canvas, bird[i]);
  }
  printBarrier(canvas, bar);

  system("clear");
  for(int y = 0; y < canvasy; y++){
    for(int x = 0; x < canvasx; x++){
      printf("%c", canvas[x][y]);
    }
    printf("\n");
  }
  if(birdsCount == 1){
    printf("Score: %d\n\nFall speed of the bird: %g\nHeight of the bird: %g\nX distance to next bar: %g\nY distance to next bar: %g\n",
        bird[0].score, bird[0].fallSpeed, bird[0].y, bird[0].xToNextBar, bird[0].yToNextBar);
  }
  else
    printf("\nHighest score: %d\n", highestScore);
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

bool ai_jump(Bird *bird){
  float decision = 0.0;
  ai_generateAiStats(bird);
  for(int j = 0; j < weightsCount; j++)
    decision += bird->ai_stats[j] * bird->weights[j];
  if(decision > 0.5 * weightsCount)
    return true;
  return false;
}

bool ai_getWeights(int batch, Bird *birds, int birdsCount){
  FILE *f;
  char *token, *line = malloc(1000);
  if(!line)
    return false;

  int hundreds = batch/100;
  int tens = (batch - 100*hundreds)/10;
  int ones = (batch - 100*hundreds - 10*tens);
  filename[14] = hundreds + '0';
  filename[15] = tens + '0';
  filename[16] = ones + '0';

  f = fopen(filename, "r");
  if(!f){
    printf("Error locating %s.\n", filename);
    free(line);
    return false;
  }

  for(int l = 0; l < birdsCount; l++){
    token = NULL;
    //Load float numbers from one line
    for(int i = 0; i < 999; i++){
      line[i] = fgetc(f);
      if(line[i] == '\n' || line[i] == EOF){
        line[i] = '\0';
        break;
      }
    }
    for(int i = 0; i < weightsCount; i++){
      if(i == 0)
        token = strtok(line, &delim);
      else
        token = strtok(NULL, &delim);
      if(token)
        birds[l].weights[i] = strtof(token, NULL);
    }
  }

  free(line);
  fclose(f);
  return true;
}

void ai_exportWeights(Bird *birds, int birdsCount){
  filename[18] = 'o';
  FILE *f = fopen(filename, "a");
  char *buffer = malloc(1000);
  for(int i = 0; i < birdsCount; i++){
    if(birds[i].score > 0){
      snprintf(buffer, 100, "%d%c", birds[i].score, delim);
      fputs(buffer, f);
      for(int j = 0; j < weightsCount; j++){
        if(j != weightsCount - 1)
          snprintf(buffer, 100, "%f%c", birds[i].weights[j], delim);
        else
          snprintf(buffer, 100, "%f", birds[i].weights[j]);
        fputs(buffer, f);
      }
      fputc('\n', f);
    }
  }
  fclose(f);
  free(buffer);
}

//Check if at least one bird is alive
void checkGameStatus(Bird *birds, bool *gameOver, int birdsCount){
  *gameOver = true;
  for(int i = 0; i < birdsCount; i++)
    if(birds[i].alive)
      *gameOver = false;
}

/*
 *
 * MAIN
 *
 */

int main(int argc, char **argv){
  int batch, birdsCount = 1, highestScore = -1;
  bool gameOver = false, ai_checkJump = true;
  if(argc < 3){
    ai = false;
  }else{
    batch = atoi(argv[1]);
    birdsCount = atoi(argv[2]);
  }

  //Init of a canvas, a barrier and birds
  char canvas[canvasx][canvasy];
  Barrier bar = {0, 0, 0, 0, 0};
  Bird *birds = malloc(birdsCount * sizeof(Bird));
  if(!birds || !birdInit(birds, birdsCount)){
    freeBirds(birds, birdsCount);
    return -1;
  }

  //Init AI weights
  if(ai && !ai_getWeights(batch, birds, birdsCount)){
    freeBirds(birds, birdsCount);
    return -1;
  }

  //Time init (for rand(), time difference calculation etc.)
  time_t t = 0;
  srand((unsigned) time(&t));
  gettimeofday(&actTime, NULL);
  gettimeofday(&lastCheck, NULL);
  u_int64_t nextUpdate = 0, 
            nextJumpCheck = 0,
            timediff = 0, 
            time = actTime.tv_usec + actTime.tv_sec*1000000;

  //Main game cycle
  while(!gameOver){
    checkBar(&bar, birds, birdsCount, &highestScore);

    for(int i = 0; i < birdsCount; i++){
      //Check, if the command to jump has been given by user (AI jump command
      //is only checked once in a while to optimise)
      if(nextUpdate != 0){
        if(!ai && jump())
          birds[i].fallSpeed = -jumpHeight;
        //Check, if AI wants to jump
        if(ai_checkJump && ai_jump(&birds[i]))
          birds[i].fallSpeed = -jumpHeight;
      }
      //Check, if bird isn't dead
      if(birds[i].alive){
        if(!checkBird(&birds[i], &bar)){
          birds[i].alive = false;
          if(!ai)
            printf("You died. Score: %d\n", birds[i].score);
          else if(ai && aiDebug)
            printf("Bird %d died. Score: %d\n", i + 1, birds[i].score);
        }
      }
    }

    //Only check if AI wants to jump every 1ms to optimise
    if(ai_checkJump){
      ai_checkJump = false;
      nextJumpCheck = time + 1000; 
    }else{
      ai_checkJump = ai && nextJumpCheck <= time;
    }

    //Only update the screen "once in a while"
    gettimeofday(&actTime, NULL);
    time = actTime.tv_usec + actTime.tv_sec*1000000;

    if(time > nextUpdate){
      nextUpdate = time + 200*speed;
      timediff = (time - (u_int64_t)lastCheck.tv_usec - (u_int64_t)lastCheck.tv_sec*1000000)/1000000;
      gettimeofday(&lastCheck, NULL);


      //Update positions and stats of birds and the bar and check for collision
      for(int i = 0; i < birdsCount; i++)
        //Calculate next position of the bird and update it's data
        updateBird(birds, bar, i, timediff);
      (bar.x1)--;
      (bar.x2)--;
      checkGameStatus(birds, &gameOver, birdsCount);

      //Print updated canvas and stats
      if(!background)
        printOut(canvas, birds, birdsCount, bar, highestScore);

      //Print stats that are fed to AI
      if(ai && aiDebug){
        for(int i = 0; i < birdsCount; i++){
          printf("Stats of bird %d: ", i);
          for(int j = 0; j < weightsCount; j++)
            printf("%g\t", birds[i].ai_stats[j]);
          printf("\n");
        }
      }
    }
  }

  //Save the weights that lead to successful game (score>0)
  if(ai)
    ai_exportWeights(birds, birdsCount);

  freeBirds(birds, birdsCount);
  return 0;
}
