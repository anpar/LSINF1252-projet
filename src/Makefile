all: jeu test

dames.o: dames.c dames.h
	gcc -std=c99 -c dames.c

programme.o: programme.c dames.h
	gcc -std=c99 -c programme.c

test.o: test.c dames.h aux.h
	gcc -std=c99 -c test.c

test: dames.o test.o
	gcc -std=c99 -o test test.o dames.o -lcunit

jeu: programme.o dames.o
	gcc -std=c99 -o programme programme.o dames.o

clean:
	rm -rf *.o

mrproper: clean
	rm dames programme test
