#include "flappy_bird.h"
#include "ai.h"

#define ai 1
#define aiDebug 0
#define background 1

int main(int argc, char **argv){
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
      if(aiDebug)
        printf("File: %s\n", filename);
      printf("birdSpeedW = %g\nbirdHeightW = %g\ncanvasHeightW = %g\ndistanceToBarW = %g\nbarHeightW = %g\n", 
          ai_birdSpeedW, ai_birdHeightW, ai_canvasHeightW, ai_distanceToBarW, ai_barHeightW);
      return 0;
    }
  }

  char canvas[canvasx][canvasy];
  Bird bird;
  if(!birdInit(&bird))
    return -1;
  Barrier bar;

  time_t t = 0;
  srand((unsigned) time(&t));
  gettimeofday(&lastCheck, NULL);
  gettimeofday(&actTime, NULL);
  u_int64_t time = actTime.tv_usec + actTime.tv_sec;
  u_int64_t nextUpdate = 0, timediff = 0;

  int score = -1;
  bool gameOver = false;

  while(1){
    if(!ai && jump() || ai && ai_jump())
      bird.fall_speed = -jumpHeight;

    if(!checkBird(bird, &bar, &score))
      break;

    gettimeofday(&actTime, NULL);
    u_int64_t time = actTime.tv_usec + actTime.tv_sec*1000000;
    if(time > nextUpdate){
      nextUpdate = time + 400*speed;

      //calculate next position of the bird - gravity
      timediff = (u_int64_t)actTime.tv_sec - (u_int64_t)lastCheck.tv_sec;
      gettimeofday(&lastCheck, NULL);
      bird.fall_speed += timediff/4.0 + 0.3;
      bird.y = bird.y + bird.fall_speed;
      ai_birdSpeed = bird.fall_speed;
      ai_birdHeight = bird.y;

      //update canvas in memory
      updateBird(&bird);
      updateBar(&bar);

      ai_distanceToBar = (bar.x1 + bar.x2)/2 - birdx;

      //print updaged canvas
      if(!background)
        printCanvas(canvas, bird, bar);
      printf("\nScore = %d\n\n", score);
      printf("Canvas Height: %d\nSpeed: %g\nHeight: %g\nDistance to next bar: %g\nNext bar height: %d\n",
          ai_canvasHeight, ai_birdSpeed, ai_birdHeight, ai_distanceToBar, ai_barHeight);
    }
  }

  if(ai && score > 0)
    ai_printWeights(score);

  freeBirdPixels(bird);
  return 0;
}

