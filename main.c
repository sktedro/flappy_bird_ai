#include "flappy_bird.h"

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
