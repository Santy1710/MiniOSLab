
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
THREADS = -pthread

SRC = $(filter-out src/demo_procesos.c src/worker.c,$(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

.PHONY: all clean test-memory test-fcfs test-rr test-deadlock test-pthreads test-processes

all: minioslab demo_procesos worker

minioslab: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o minioslab $(THREADS) -lm

worker: src/worker.c
	$(CC) $(CFLAGS) src/worker.c -o worker -lm

demo_procesos: src/demo_procesos.c src/procesos.c src/procesos.h worker
	$(CC) $(CFLAGS) src/demo_procesos.c src/procesos.c -o demo_procesos $(THREADS)

test-memory: minioslab
	./minioslab --memory-test

test-fcfs: minioslab
	./minioslab data/scheduler.txt --scheduler fcfs

test-rr: minioslab
	./minioslab data/scheduler.txt --scheduler rr --quantum 2

test-deadlock: minioslab
	./minioslab --deadlock-test

test-pthreads: minioslab
	./minioslab --unsafe
	./minioslab --safe

test-processes: demo_procesos
	./demo_procesos data/trabajos.txt --seq
	./demo_procesos data/trabajos.txt --par

clean:
	rm -f minioslab demo_procesos worker
