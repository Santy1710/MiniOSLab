
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
THREADS = -pthread

# Archivos del programa principal.
# Excluimos los programas que tienen su propio main().
SRC = $(filter-out src/demo_procesos.c src/worker.c,$(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

.PHONY: all clean test-memory test-fcfs test-rr test-deadlock

# Compilacion principal.
all: minioslab

minioslab: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o minioslab $(THREADS)

# Pruebas individuales.
test-memory: minioslab
	./minioslab --memory-test

test-fcfs: minioslab
	./minioslab trabajos.txt --scheduler fcfs

test-rr: minioslab
	./minioslab trabajos.txt --scheduler rr --quantum 2

test-deadlock: minioslab
	./minioslab --deadlock-test

# Eliminar ejecutables generados.
clean:
	rm -f minioslab demo_procesos worker
