CFLAGS = -std=c99 -Wall

all: upfact

upfact: upfact.o io.o perf.o util.o
	gcc $(CFLAGS) -o upfact upfact.o io.o perf.o util.o

upfact.o: upfact.c io.h perf.h
	gcc $(CFLAGS) -c upfact.c

io.o: io.c io.h
	gcc $(CfLAGS) -c io.c

perf.o: perf.c perf.h
	gcc $(CFLAGS) -c perf.c

util.o: util.c util.h
	gcc $(CFLAGS) -c util.c

clean:
	rm -rf *.o

mr-proper: clean
	rm upfact
	rm *.c~
	rm *.h~
