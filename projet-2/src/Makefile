CFLAGS = -std=c99 -Wall -pg

all: upfact

upfact: upfact.o io.o perf.o util.o core.o stack.o squfof.o fopen.o trial.o
	gcc $(CFLAGS) -o upfact upfact.o io.o perf.o util.o core.o stack.o squfof.o fopen.o trial.o -lm -lpthread -lcurl 

upfact.o: upfact.c io.h perf.h util.h core.h stack.h squfof.o
	gcc $(CFLAGS) -c upfact.c

io.o: io.c io.h
	gcc $(CFLAGS) -c io.c

perf.o: perf.c perf.h
	gcc $(CFLAGS) -c perf.c

util.o: util.c util.h
	gcc $(CFLAGS) -c util.c

core.o: core.c core.h
	gcc $(CFLAGS) -c core.c

stack.o: stack.c stack.h
	gcc $(CFLAGS) -c stack.c

squfof.o: squfof.c squfof.h
	gcc $(CFLAGS) -c squfof.c

fopen.o: fopen.c fopen.h
	gcc $(CFLAGS) -c fopen.c 

trial.o: trial.c trial.h
	gcc $(CFLAGS) -c trial.c

clean:
	rm -rf *.o

mr-proper: clean
	rm -rf upfact
	rm -rf *.c~
	rm -rf *.h~
