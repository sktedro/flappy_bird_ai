ALL: 
	mkdir -p build
	mkdir -p data
	gcc -Wall game.c settings.h -o build/game -g -lncurses
	gcc -Wall sort.c settings.h -o build/sort -g
	gcc -Wall wgen.c settings.h -o build/wgen -g -lm

profile:
	mkdir -p build
	mkdir -p data
	gcc -pg game.c settings.h -o build/game -g -lncurses
	gcc -pg sort.c settings.h -o build/sort -g
	gcc -pg wgen.c settings.h -o build/wgen -g -lm

check:
	mkdir -p build
	mkdir -p data
	gcc -Wall -Wextra -Werror -pedantic game.c settings.h -o build/game -g -lncurses
	gcc -Wall -Wextra -Werror -pedantic -pg sort.c settings.h -o build/sort -g
	gcc -Wall -Wextra -Werror -pedantic -pg wgen.c settings.h -o build/wgen -g -lm

clean:
	rm -rf build data profiles
