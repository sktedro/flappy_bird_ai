#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <stdbool.h> 
#include <time.h> 
#include <sys/time.h> 
#include <curses.h> 
#include <math.h> 

#include "ai_data.h"
#include "settings.h"


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
  ai_barHeight = rand()%(canvasy - 2*barGap) + barGap;
  bar->highy = ai_barHeight + barGap/2;
  bar->lowy = ai_barHeight - barGap/2;
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
