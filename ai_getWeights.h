float ai_birdSpeedW, ai_birdHeightW, ai_canvasHeightW, ai_distanceToBarW, ai_barHeightW;

bool ai_getWeights(int batch, int setting){
  FILE *f;
  char *buffer;
  size_t bufferSize;
  char filename[] = "./data/weightsXXX_input";
  if(batch == 1){
    filename[14] = filename[15] = '0';
    filename[16] = '1';
  }else if(batch == 2){

  }

  f = fopen(filename, "r");
  if(!f){
    printf("Error locating %s.\n", filename);
    fclose(f);
    return false;
  }

  //Get to the desired line
  for(int i = 0; i < setting - 1; i++){
    char c = ' ';
    while(c != '\n' && c != EOF)
      c = fgetc(f);
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
