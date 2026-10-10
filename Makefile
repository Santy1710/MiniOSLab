CC = gcc
CFLAGS = -Wall -Wextra -pthread

SRC = src/main.c \
      src/scheduler.c \
      src/deadlock.c

OUT = minioslab

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

clean:
	rm -f $(OUT)