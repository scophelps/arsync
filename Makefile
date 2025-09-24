CC=gcc
CFLAGS=-Wall -Wextra -O2
LFLAGS=-lpthread

SRC=src
OUT=out
OBJ=$(OUT)/obj
BIN=$(OUT)/bin
RES=$(BIN)/arsync

all: $(RES)

clean:
	rm -f $(OUT)/*/*

run: $(RES)
	$(RES)

install: $(RES)
	sudo cp -f $(RES) ~/bin/

$(RES): $(OBJ)/arsync.o
	$(CC) $(LFLAGS) $^ -o $@

$(OBJ)/arsync.o: $(SRC)/arsync.c
	$(CC) -c $(CFLAGS) $^ -o $@

$(SRC)/%.c: $(SRC)/%.h
