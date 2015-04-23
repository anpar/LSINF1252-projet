CFLAGS = -std=c99 -Wall -g

all: upfact

upfact: upfact.o io.o perf.o util.o core.o stack.o
	gcc $(CFLAGS) -o upfact upfact.o io.o perf.o util.o core.o stack.o -lpthread

upfact.o: upfact.c io.h perf.h util.h core.h
	gcc $(CFLAGS) -c upfact.c

io.o: io.c io.h
	gcc $(CFLAGS) -c io.c

perf.o: perf.c perf.h
	gcc $(CFLAGS) -c perf.c

util.o: util.c util.h
	gcc $(CFLAGS) -c util.c

core.o: core.c core.h
	gcc $(CFLAGS) -c core.c

stack.o: stack.c stack.o
	gcc $(CFLAGS) -c stack.c

clean:
	rm -rf *.o

mr-proper: clean
	rm upfact
	rm *.c~
	rm *.h~
