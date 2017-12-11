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
#include "Connection.h"


#define TRUE   1
#define FALSE  0

class Sockets{
private:

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

public:
  Connection &connection;
  Sockets(Connection  &connectIn);
  void programmThread();
};



#endif
