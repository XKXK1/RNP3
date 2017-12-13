#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <thread>
#include "Connection.h"
#include <sys/time.h>
#include "Sockets.h"
#include <ctime>


#define TRUE   1
#define FALSE  0

class Control{
private:
  void sendConMsg(int sock,uint32_t hashCode, uint16_t port, char name[]);

  Connection &connection;
  Sockets &socket1;

  std::thread peerThread;
  std::string userInput;

  void generateHash();

  bool messaging;

  /*
  getting input for listening port and optional
  connecting port then starting the Socket Thread
  */
  void controlSetup();

  /*
  getting Input for sending a Message
  */
  void getInputForMessage(uint32_t hashCode);

  /*
  finally send Message out to chosen socket
  */
  void sendMessage();

  int sock = 0;
  int listenPort = 0;

  struct protocolMessage message;

public:
  Control(Connection &connectIn, Sockets &socketIn);

  /*
  Thread that handles all incoming input(Messages,help,stop)
  */
  void controller();
};



#endif
