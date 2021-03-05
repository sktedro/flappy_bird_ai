#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <curses.h>
#include <math.h>

#define gravity 0.05
#define speed 100 //default: 100 - the higher the slower
#define jumpHeight 2

#define canvasx 100 //canvas width
#define canvasy 35 //canvas height

#define birdx 10 //bird location from the left
#define birdPixels 14 //a constant

#define barWidth 8 //width of a single barrier
#define barGap 10 //gap between top and bottom barriers

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

void newBar(Barrier *bar){
  bar->highy = rand()%(canvasy - 2*barGap) + barGap;
  bar->lowy = bar->highy - barGap;
  bar->x1 = bar->x1 = canvasx - 1;
  bar->x2 = bar->x2 = canvasx + barWidth;
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

void updateBird(Bird *bird){
  bird->pixels[0][1] = bird->pixels[1][1] = bird->pixels[2][1] = bird->pixels[3][1] = bird->y;
  bird->pixels[4][1] = bird->pixels[5][1] = bird->pixels[6][1] = bird->pixels[7][1] =
    bird->pixels[12][1] = bird->y+1;
  bird->pixels[8][1] = bird->pixels[9][1] = bird->pixels[10][1]= bird->pixels[11][1]=
    bird->pixels[13][1] = bird->y-1;
}

void updateBar(Barrier *bar){
  (bar->x1)--;
  (bar->x2)--;
  (bar->x1)--;
  (bar->x2)--;
}

void freeBirdPixels(Bird bird){
  for(int i = 0; i < birdPixels; i++)
    free(bird.pixels[i]);
  free(bird.pixels);
}

int main(){
  char canvas[canvasx][canvasy];
  Bird bird;
  if(!birdInit(&bird))
    return -1;
  Barrier bar;

  time_t t = 0;
  srand((unsigned) time(&t));
  gettimeofday(&lastCheck, NULL);
  gettimeofday(&actTime, NULL);
  uint64_t time = actTime.tv_usec + actTime.tv_sec;
  uint64_t nextUpdate = 0, timediff = 0;

  int score = -1;
  bool gameOver = false;

  while(1){
    if(jump())
      bird.fall_speed = -jumpHeight;

    if(!checkBird(bird, &bar, &score))
      break;

    gettimeofday(&actTime, NULL);
    uint64_t time = actTime.tv_usec + actTime.tv_sec*1000000;
    if(time > nextUpdate){
      nextUpdate = time + 400*speed;

      //calculate next position of the bird - gravity
      timediff = (uint64_t)actTime.tv_sec - (uint64_t)lastCheck.tv_sec;
      gettimeofday(&lastCheck, NULL);
      bird.fall_speed += timediff/4 + 0.3;
      bird.y = bird.y + bird.fall_speed;

      //update canvas in memory
      updateBird(&bird);
      updateBar(&bar);

      //print updaged canvas
      printCanvas(canvas, bird, bar);
      printf("Speed: %g\nY coordinate: %g\nScore = %d\n", bird.fall_speed, bird.y, score);
    }
  }
  freeBirdPixels(bird);
  return 0;
}
