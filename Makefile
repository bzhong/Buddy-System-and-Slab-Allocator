CC = g++
DEBUG = -g
CFLAGS = -std=c++0x -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
OBJ = main.o buddy.o command.o

P2: main.o buddy.o
	$(CC) $(LFLAGS) $(OBJ) -o P2

main.o: main.cpp buddy.h
	$(CC) $(CFLAGS) main.cpp buddy.cpp command.cpp

buddy.o: buddy.h common.cpp
	$(CC) $(CFLAGS) buddy.cpp common.cpp

command.o: command.h
	$(CC) $(CFLAGS) command.cpp

clean:
	rm -rf P2 *.o
