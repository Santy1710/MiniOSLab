CC     = gcc
CFLAGS = -Wall -Wextra -g -pthread
LDLIBS = -lm

# Incluye main.c y tu módulo pthreads.c
SRC = src/main.c \
      src/pthreads.c

OUT = minioslab

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDLIBS)

clean:
	rm -f $(OUT)
