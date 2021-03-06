ALL: 
	mkdir -p build
	mkdir -p data
	gcc game.c -o build/game -g -lncurses
	gcc sort.c -o build/sort -g
	gcc wgen.c -o build/wgen -g -lm
