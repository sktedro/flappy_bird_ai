#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
#include <math.h>


#define ai 0 //Toggle AI instead of the user playing the game
#define aiDebug 0 //Will print AI weights
#define background 0 //Toggle running on background (only data, no "images")

#define canvasx 100 //Canvas width
#define canvasy 35 //Canvas height

#define speed 100 //Flight speed (x direction) - the higher the slower

#define jumpHeight 2.0

#define birdx 10 //Bird location from the left
#define birdPixels 14 //Don't mind this

#define barWidth 8 //Width of a single barrier
#define barGap 10 //Gap between top and bottom barriers


int canvasHeight, barHeight;

char filename[] = "./data/weightsXXX_i";

enum wIndex{birdSpeedWI, birdHeightWI, canvasHeightWI, distanceToBarWI, barHeightWI};
enum statIndex{birdSpeedI, birdHeightI, distanceToBarI};

struct timeval lastCheck, actTime;

typedef struct{
  bool alive;
  int score;
  float y;
  float fall_speed;
  int **pixels;
  float weights[5];
  float stats[3];
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
  barHeight = rand()%(canvasy - 2*barGap) + barGap;
  bar->highy = barHeight + barGap/2;
  bar->lowy = barHeight - barGap/2;
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

bool birdInit(Bird *bird, int birdsCount){
  for(int i = 0; i < birdsCount; i++){
    bird[i].alive = true;
    bird[i].score = -1;
    bird[i].y = (int)canvasy/2;
    bird[i].fall_speed = 0;
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
  //TODO fuj to pole. Nechcem ho deklarovat tak casto
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

void checkBar(Barrier *bar, Bird *birds, int birdsCount){
  if(bar->x2 < 0){
    newBar(bar);
    for(int i = 0; i < birdsCount; i++){
      if(birds[i].alive){
        (birds[i].score)++;
      }
    }
  }
}

bool checkBird(Bird *bird, Barrier *bar){
  if(bird->y < 0 || bird->y > canvasy){
    return false;
  }
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

void updateBird(Bird *bird){
  bird->pixels[0][1] = bird->pixels[1][1] = bird->pixels[2][1] = bird->pixels[3][1] =
    bird->y;
  bird->pixels[4][1] = bird->pixels[5][1] = bird->pixels[6][1] = bird->pixels[7][1] =
    bird->pixels[12][1] = bird->y+1;
  bird->pixels[8][1] = bird->pixels[9][1] = bird->pixels[10][1]= bird->pixels[11][1]=
    bird->pixels[13][1] = bird->y-1;
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

void printCanvas(char canvas[canvasx][canvasy], Bird *bird, int birdsCount, Barrier bar){
  for(int i = 0; i < canvasx; i++)
    for(int j = 0; j < canvasy; j++)
      canvas[i][j] = ' ';
  for(int i = 0; i < birdsCount; i++)
    printBird(canvas, bird[i]);
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

bool ai_jump(Bird *birds, int i){
  float decision =
    birds[i].stats[birdSpeedI] * birds[i].weights[birdSpeedWI] +
    birds[i].stats[birdHeightI] * birds[i].weights[birdHeightWI]/10 +
    canvasHeight * birds[i].weights[canvasHeightWI]/50 +
    birds[i].stats[distanceToBarI] * birds[i].weights[distanceToBarWI]/20 +
    barHeight * birds[i].weights[barHeightWI]/10;
  if(decision > 0.5 * 5)
    return true;
  return false;
}

bool ai_getWeights(int batch, Bird *birds, int birdCount){
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

  /*
  //Get to the desired line
  for(int i = 0; i < setting - 1; i++){
    char c = ' ';
    while(c != '\n'){
      c = fgetc(f);
      if(c == EOF)
        break;
    }
  }
  */

  char *line = malloc(1000);
  char delim = ';';
  char *token;

  for(int l = 0; l < birdCount; l++){
    //Get one line
    for(int i = 0; i < 999; i++){
      line[i] = fgetc(f);
      if(line[i] == '\n' || line[i] == EOF)
        break;
    }
    //printf("%s", line);

    //Get floats from that line
    token = strtok(line, &delim); //Skip the first record of the line - score
    if(!token || token[0] == EOF)
      return false;
    birds[l].weights[birdSpeedWI] = strtof(token, NULL);
    token = strtok(NULL, &delim);
    birds[l].weights[birdHeightWI] = strtof(token, NULL);
    token = strtok(NULL, &delim);
    birds[l].weights[canvasHeightWI] = strtof(token, NULL);
    token = strtok(NULL, &delim);
    birds[l].weights[distanceToBarWI] = strtof(token, NULL);
    token = strtok(NULL, &delim);
    birds[l].weights[barHeightWI] = strtof(token, NULL);
  }

  free(line);
  fclose(f);
  return true;
}

void ai_printWeights(Bird *birds, int birdsCount){
  filename[18] = 'o';
  FILE *f = fopen(filename, "a");
  char *buffer = malloc(1000);
  for(int i = 0; i < birdsCount; i++){
    if(birds[i].score > 0){
      snprintf(buffer, 100, "%d;", birds[i].score);
      fputs(buffer, f);
      snprintf(buffer, 100, "%f;", birds[i].weights[birdSpeedWI]);
      fputs(buffer, f);
      snprintf(buffer, 100, "%f;", birds[i].weights[birdHeightWI]);
      fputs(buffer, f);
      snprintf(buffer, 100, "%f;", birds[i].weights[canvasHeightWI]);
      fputs(buffer, f);
      snprintf(buffer, 100, "%f;", birds[i].weights[distanceToBarWI]);
      fputs(buffer, f);
      snprintf(buffer, 100, "%f", birds[i].weights[barHeightWI]);
      fputs(buffer, f);
      fputc('\n', f);
    }
  }
  fclose(f);
  free(buffer);
}

/*
 *
 * MAIN
 *
 */

int main(int argc, char **argv){
  int batch, setting, birdsCount = 1; //TODO remove setting var

  //Init of a canvas, birds and a barrier
  char canvas[canvasx][canvasy];
  canvasHeight = canvasy;

  Bird *birds = malloc(birdsCount * sizeof(Bird));
  if(!birds)
    return -1;
  if(!birdInit(birds, birdsCount))
    return -1;
  Bird bird;

  Barrier bar;

  //AI initialisation
  if(ai){
    if(argc < 3){
      printf("main.c: Too few arguments\n");
      return -1;
    }
    batch = atoi(argv[1]);
    birdsCount = atoi(argv[3]);
    for(int i = 0; i < birdsCount; i++){
      if(!ai_getWeights(batch, birds, birdsCount))
        return -1;
    }
    if(aiDebug){
      printf("File: %s\n", filename);
    }
  }


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
    for(int i = 0; i < birdsCount; i++){
      //Check, if the command to jump has been given
      if((!ai && jump()) || (ai && ai_jump(birds, i)))
        birds[i].fall_speed = -jumpHeight;
      //Check, if bird isn't dead
      if(birds[i].alive){
        if(!checkBird(&birds[i], &bar)){
          birds[i].alive = false;
          printf("Bird %d died. Score: %d\n", i, birds[i].score);
        }
      }
    }
    checkBar(&bar, birds, birdsCount);

    //Only update the screen "once in a while"
    gettimeofday(&actTime, NULL);
    u_int64_t time = actTime.tv_usec + actTime.tv_sec*1000000;
    if(time > nextUpdate){
      nextUpdate = time + 400*speed;

      //Calculate next position of the bird while keeping gravity in mind
      timediff = (u_int64_t)actTime.tv_sec - (u_int64_t)lastCheck.tv_sec;
      gettimeofday(&lastCheck, NULL);
      for(int i = 0; i < birdsCount; i++){
        birds[i].fall_speed += timediff/4.0 + 0.3;
        birds[i].y = birds[i].y + birds[i].fall_speed;

        birds[i].stats[birdSpeedI] = birds[i].fall_speed;
        birds[i].stats[birdHeightI] = birds[i].y;
        birds[i].stats[distanceToBarI] = (bar.x1 + bar.x2)/2 - birdx;

        //Update canvas in memory
        updateBird(&birds[i]);
        updateBar(&bar);
      }

      //Print updated canvas
      if(!background){
        printCanvas(canvas, birds, birdsCount, bar);
      }

      //Print stats
      if(!background && birdsCount == 1){
        printf("Score = %d\n", birds[0].score);
        printf("Canvas Height: %d\nSpeed: %g\nHeight: %g\nDistance to next bar: %g\nNext bar height: %d\n",
            canvasHeight, birds[0].stats[birdSpeedI], 
            birds[0].stats[birdHeightI], birds[0].stats[distanceToBarI], 
            barHeight);
      }

      //Check if a bird is alive
      gameOver = true;
      for(int i = 0; i < birdsCount; i++)
        if(birds[i].alive)
          gameOver = false;
      if(birds[0].alive)
        gameOver = false;
    }
  }

  //Save the weights that lead to successful game (score>0)
  if(ai)
    ai_printWeights(birds, birdsCount);

  freeBirds(birds, birdsCount);
  return 0;
}
