#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sstream>
#include <errno.h>
#include <string>
#include <thread>
#include <vector>

#include <mutex>

#define CON_TYPE 1
#define SYN_TYPE 2
#define MSG_TYPE 3

#define TRUE   1
#define FALSE  0


struct ip_port_data{
  uint32_t ip;
  uint16_t port;
  uint16_t hasNext;
};

struct protocolMessage{
  uint16_t version;
  uint16_t type;
  uint32_t hashCode;
  union{
    struct{
      char name[16];
      struct ip_port_data connections[37];
    };
    char text[312];
  }data;
};

struct socketData{
  struct protocolMessage message;
  int sd = -1;
};

struct clientSocket{
  char name[16];
  uint32_t hashCode;
  uint32_t ip;
  uint16_t port;
  int sd = -1;
};

class Connection{
private:

  //connect to a listening Port
  int connect_socket();

  //create listening Port
  void createMasterSocket();
  //specify socket options
  void initialiseSocketType(int port);
  //binding listening socket to listening port
  void bindSocketToPort();

  void specifyMaxConMasterSocket();

public:
  std::mutex mut;
  Connection();
  ~Connection();
  //information about master
  struct clientSocket master;
  //holds all other sockets
  std::vector<struct clientSocket> sockets;
  int listenPort;
  struct sockaddr_in address;

  int connect_socket(int connect_port, char *address);
  void initProgramm(int port);

  int addrlen;
  int opt = TRUE;
};



#endif
