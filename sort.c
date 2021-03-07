#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define weightsCount 5

typedef struct{
  int score;
  char *content;
} Line;

//Counts lines in the file
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

//Loads raw lines
bool readLines(Line *line, FILE *f, int lines){
  int lineLen = 100;
  for(int l = 0; l < lines; l++){
    line[l].content = malloc(lineLen);
    if(!line[l].content)
      return false;
    line[l].content[0] = '\0';

    for(int i = 0; ; i++){
      if(i >= lineLen - 2){
        lineLen += 10;
        void *p = realloc(line[l].content, lineLen);
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

//Reads first column of the file and writes the number into line[].score
void getScores(Line *line, int lines){
  char *scoreStr = malloc(100);
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

//Bubblesort the lines based on score (highest score first)
void sort(Line *line, int len){
  int i, j;
  for (i = 0; i < len-1; i++)
    for (j = 0; j < len-i-1; j++)
      if(line[j].score < line[j+1].score)
        swap(line, j, j+1);
}

//Export the sorted lines into the file
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
  if(argc < 2){
    printf("./sort [path to file]\n");
    printf("eg.: './sort ../data/weights001.o'\n");
    return -1;
  }

  //Open the file and load raw lines
  FILE *f = fopen(argv[1], "r");
  if(!f){
    printf("File not found\n");
    return -1;
  }
  int lines = getFileLength(f);
  Line *line = malloc((lines + 1) * sizeof(Line));
  if(!line)
    return -1;
  if(!readLines(line, f, lines)){
    freeAll(line, lines);
    return -1;
  }
  fclose(f);

  //Open the file, sort the lines and print them to the file
  f = fopen(argv[1], "w");
  if(!f){
    printf("File not found\n");
    freeAll(line, lines);
    return -1;
  }
  getScores(line, lines);
  sort(line, lines);
  printToFile(line, lines, f);
  fclose(f);

  freeAll(line, lines);
}
