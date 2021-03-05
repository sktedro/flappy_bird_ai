float ai_birdSpeedW, ai_birdHeightW, ai_canvasHeightW, ai_distanceToBarW, ai_barHeightW;

char filename[] = "./data/weightsXXX_input";

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

  char *line = malloc(1000);
  for(int i = 0; i < 1000; i++){
    line[i] = fgetc(f);
    if(line[i] == '\n' || line[i] == EOF)
      break;
  }

  //printf("%s", line);

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
