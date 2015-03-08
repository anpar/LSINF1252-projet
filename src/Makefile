all: test.c programme.c
	gcc -o test test.c -lcunit
	gcc -o programme programme.c
	gcc -o dames dames.c

test: test.c
	gcc -o test test.c -lcunit

jeu: programme.c
	gcc -o programme programme.c
