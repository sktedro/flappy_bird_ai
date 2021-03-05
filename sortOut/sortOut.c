#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct{
  int score;
  char *content;
} Line;

int getFileLength(FILE *f){
  int count = 0;
  char c;
  while(1){
    c = fgetc(f);
    if(c == '\n')
      count++;
    if(c == EOF){
      break;
    }
  }
  fseek(f, 0, SEEK_SET);
  return count;
}

bool readLines(Line *line, FILE *f, int lines){
  int lineLen;
  for(int l = 0; l < lines; l++){
    lineLen = 100;
    line[l].content = malloc(lineLen);
    if(!line[l].content)
      return false;
    line[l].content[0] = '\0';

    for(int i = 0; ; i++){
      if(i >= lineLen - 2){
        lineLen += 10;
        char *p = realloc(line[l].content, lineLen);
        if(!p)
          return false;
        line[l].content = p;
      }
      line[l].content[i] = fgetc(f);
      line[l].content[i+1] = '\0';
      if(line[l].content[i] == '\n' || line[l].content[i] == EOF)
        break;
    }
  }
  return true;
}

void getScores(Line *line, int lines){
  char *scoreStr = malloc(10);
  for(int i = 0; i < lines; i++){
    for(int j = 0; line[i].content[j] != '\n' && line[i].content[j] != EOF; j++){
      scoreStr[j] = line[i].content[j];
      if(line[i].content[j] == ';'){
        line[i].score = strtol(scoreStr, NULL, 10);
        break;
      }
    }
  }
  free(scoreStr);
}

void swap(Line *line, int i1, int i2){
  char *tmpStr = line[i1].content;
  int tmpInt = line[i1].score;

  line[i1].content = line[i2].content;
  line[i1].score = line[i2].score;

  line[i2].content = tmpStr;
  line[i2].score = tmpInt;
}

void sort(Line *line, int len){
  int i, j;
  for (i = 0; i < len-1; i++)
    for (j = 0; j < len-i-1; j++)
      if(line[j].score < line[j+1].score)
        swap(line, j, j+1);
}

void printToFile(Line *line, int lines, FILE *f){
  for(int i = 0; i < lines; i++)
    fputs(line[i].content, f);
}

void freeAll(Line *line, int lines){
  for(int i = 0; i < lines; i++)
    free(line[i].content);
  free(line);
}

int main(int argc, char **argv){
  if(argc < 3){
    printf("./sortOut [path to input file] [path to output file]\n");
    printf("eg.: './sortOut ../data/weights001.o ../data/weights002.i'\n");
    return -1;
  }
  printf("%s\n", argv[1]);
  FILE *f = fopen(argv[1], "r");
  if(!f){
    printf("File not found\n");
    return -1;
  }
  int lines = getFileLength(f);
  //printf("Lines: %d\n", lines);
  Line *line = malloc((lines + 1) * sizeof(Line));
  if(!line)
    return -1;
  if(!readLines(line, f, lines))
    return -1;
  fclose(f);

  getScores(line, lines);
  
  sort(line, lines);


  f = fopen(argv[2], "w");
  if(!f){
    printf("File not found\n");
    return -1;
  }

  printToFile(line, lines, f);

  fclose(f);

  freeAll(line, lines);
}
