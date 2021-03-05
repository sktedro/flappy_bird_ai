ALL: main.c
	gcc main.c flappy_bird.h settings.h -o main -g -lncurses
