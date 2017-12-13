CC = g++
CFLAGS = -g -pthread -std=gnu++11
NAME = p2p

all: Main.cpp
	 $(CC) $(CFLAGS) -o $(NAME)  *.h *.cpp

 clean:
	 $(RM) $(NAME)
