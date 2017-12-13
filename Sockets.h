#ifndef SOCKETS_H
#define SOCKETS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <thread>
#include <sys/time.h>
#include "Connection.h"


#define TRUE   1
#define FALSE  0

class Sockets{
private:
  void prepareSynMsg(int index,uint32_t ip, uint16_t port, uint32_t hashCode, char name[]);
  void sendConMsg(int sock,uint32_t hashCode, uint16_t port, char name[]);
  void handleMessageCON(int index);
  void handleMessageSYN(int index);
  void handleMessgageMSG(int index);

  /*
  Every active socket descriptor is added
  to the select set to be checked for input
  */
  void setSD();

  //set of file descriptors
  fd_set readfds;

  int max_sd,sd,valread,new_socket,activity;
  bool sdAlreadyListed = false;


  struct protocolMessage message;
  struct timeval tv;


public:
  bool threadRunning = true;
  Connection &connection;
  Sockets(Connection  &connectIn);
  void programmThread();
};



#endif
