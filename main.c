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
#define speed 100 //default: 100

#define canvasx 100 //canvas width
#define canvasy 35 //canvas height

#define birdx 10 //bird location from the left
#define birdpixels 14 //a constant

#define barWidth 8 //width of a single barrier
#define barGap 10 //gap between top and bottom barriers

bool barStatus = false;

struct timeval lastCheck, actTime;


typedef struct{
  float y;
  float fall_speed;
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
  int pixels[birdpixels][2] = {
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
  for(int i = 0; i < birdpixels; i++){
    for(int j = 0; j < 2; j++){
      canvas[pixels[i][0]][pixels[i][1]] = '%';
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

int main(){
  char canvas[canvasx][canvasy];

  Bird bird;
  bird.y = (int)canvasy/2;
  bird.fall_speed = 0;
  int pixels[birdpixels][2] = { //pixels that the bird consists of
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

  Barrier bar = {0};

  time_t t = 0;
  srand((unsigned) time(&t));
  gettimeofday(&lastCheck, NULL);

  bool gameOver = false;
  int score = -1;
  
  while(1){
    //React if enter was pressed
    initscr();
    timeout(1);
    if(getch() != ERR)
      bird.fall_speed = -2;
    endwin();
    
    //Pixels of a bird
    pixels[0][1] = pixels[1][1] = pixels[2][1] = pixels[3][1] = bird.y;
    pixels[4][1] = pixels[5][1] = pixels[8][1] = 
      pixels[9][1] = pixels[12][1] = bird.y+1;
    pixels[10][1] = pixels[11][1] = pixels[6][1] = 
      pixels[7][1] = pixels[13][1] = bird.y-1;

    if(bird.y < 0 || bird.y > canvasy){
      printf("You lost. Score: %d\n", score);
      gameOver = true;
    }
    for(int i = 0; i < birdpixels; i++){
      for(int j = 0; j < 2; j++){
        if(pixels[i][0] >= bar.x1 && pixels[i][0] <= bar.x2){
          if(pixels[i][1] <= bar.lowy || pixels[i][1] >= bar.highy){
            printf("You lost. Score: %d\n", score);
            gameOver = true;
            i = j = 1000;
          }
        }    
      }
    }
    
    if(gameOver)
      break;

    if(!barStatus){
      bar.highy = rand()%(canvasy - 2*barGap) + barGap;
      bar.lowy = bar.highy - barGap;
      bar.x1 = bar.x1 = canvasx - 1;
      bar.x2 = bar.x2 = canvasx + barWidth;
      barStatus = true;
      score++;
    }
    if(bar.x2 < 0)
      barStatus = false;


    gettimeofday(&actTime, NULL);
    if((double)actTime.tv_usec < (double)lastCheck.tv_usec){
      lastCheck.tv_usec = 0;
    }

    if((double)actTime.tv_usec > (double)lastCheck.tv_usec + 400*speed){
      int timediff = (double)actTime.tv_sec - (double)lastCheck.tv_sec; 

      bird.fall_speed += timediff/4 + 0.3;

      gettimeofday(&lastCheck, NULL);

      bird.y = bird.y + bird.fall_speed;

      (bar.x1)--;
      (bar.x2)--;
      (bar.x1)--;
      (bar.x2)--;

      printCanvas(canvas, bird, bar);

      printf("Speed: %g\nY coordinate: %g\nScore = %d\n", bird.fall_speed, bird.y, score);
    }
  }
  return 0;
}
