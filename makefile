ALL: 
	mkdir -p build
	mkdir -p data
	gcc game.c settings.h -o build/game -g -lncurses
	gcc sort.c settings.h -o build/sort -g
	gcc wgen.c settings.h -o build/wgen -g -lm
