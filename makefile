CC = g++
CFLAGS = -g -pthread -std=gnu++11 

all: Main.cpp
	 $(CC) $(CFLAGS) -o P2P  Connection.h Connection.cpp  Control.h Control.cpp Sockets.h Sockets.cpp Main.cpp

 clean:
	 $(RM) P2P
