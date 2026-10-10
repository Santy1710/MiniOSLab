CC      = gcc
CFLAGS  = -Wall -Wextra -g -O0 -std=gnu11
LDLIBS  = -lm

all: worker demo_procesos

worker: src/worker.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

demo_procesos: src/demo_procesos.c src/procesos.c src/procesos.h
	$(CC) $(CFLAGS) -o $@ src/demo_procesos.c src/procesos.c

clean:
	rm -f worker demo_procesos
