CC     = gcc
CFLAGS = -Wall -Wextra -std=c11
THREADS = -pthread

# Archivos del programa principal.
# Se incluyen automáticamente todos los .c de src/ (incluyendo pthreads.c)
# y se excluyen los que tienen su propio main() individual.
SRC     = $(filter-out src/demo_procesos.c src/worker.c,$(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

.PHONY: all clean test-memory test-fcfs test-rr test-deadlock test-pthreads

# Compilación principal de minioslab
all: minioslab

minioslab: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(SRC) -o minioslab $(THREADS) -lm

# Pruebas individuales de cada módulo
test-memory: minioslab
	./minioslab --memory-test

test-fcfs: minioslab
	./minioslab trabajos.txt --scheduler fcfs

test-rr: minioslab
	./minioslab trabajos.txt --scheduler rr --quantum 2

test-deadlock: minioslab
	./minioslab --deadlock-test

test-pthreads: minioslab
	./minioslab --unsafe
	./minioslab --safe

# Limpieza de ejecutables
clean:
	rm -f minioslab demo_procesos worker