ALL: 
	gcc main.c -o build/main -g -lncurses
	gcc sort.c -o build/sort -g
	gcc wgen.c -o build/wgen -g -lm
