CFLAGS = -std=c99 -Wall -g 

all: upfact

curl: 
	tar xjvf curl-7.42.1.tar.bz2
	cd curl-7.42.1 && ./configure --prefix=$(HOME)/local && make && make install

upfact: upfact.o io.o perf.o core.o stack.o fopen.o trial.o
	gcc $(CFLAGS) -o upfact upfact.o io.o perf.o core.o stack.o fopen.o trial.o -lm -lpthread -lcurl 

upfact.o: upfact.c io.h perf.h core.h stack.h 
	gcc $(CFLAGS) -c upfact.c

io.o: io.c io.h
	gcc $(CFLAGS) -c io.c

perf.o: perf.c perf.h
	gcc $(CFLAGS) -c perf.c

core.o: core.c core.h
	gcc $(CFLAGS) -c core.c

stack.o: stack.c stack.h
	gcc $(CFLAGS) -c stack.c

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
