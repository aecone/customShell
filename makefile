CC=gcc
CFLAGS=-Wall -g
LDFLAGS=
OBJ=main.o parse.o execute.o utils.o
EXEC=mysh

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

main.o: main.c parse.h execute.h utils.h
	$(CC) $(CFLAGS) -c main.c

parse.o: parse.c parse.h
	$(CC) $(CFLAGS) -c parse.c

execute.o: execute.c execute.h
	$(CC) $(CFLAGS) -c execute.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f $(OBJ) $(EXEC)
