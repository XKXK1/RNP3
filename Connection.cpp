#include "Connection.h"


Connection::Connection(){
}

int Connection::connect_socket(int connect_port, char *address){
  std::cout << "You are connecting to port: "<< connect_port << " ip: " << address << std::endl;
  struct sockaddr_in addr;
  int cfd;

  cfd = socket(AF_INET, SOCK_STREAM, 0);
  if (cfd == -1) {
    perror("socket");
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_port = htons(connect_port);
  addr.sin_family = AF_INET;

  if (!inet_aton(address, (struct in_addr *) &addr.sin_addr.s_addr)) {
    perror("bad IP address format");
    close(cfd);
    return -1;
  }

  if (connect(cfd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
    perror("connect()");
    shutdown(cfd, SHUT_RDWR);
    close(cfd);
    return -1;
  }
  return cfd;
}


void Connection::createMasterSocket(){
  //create a master socket
  if( (master.sd = socket(AF_INET , SOCK_STREAM , 0)) == 0){
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  // for reusability of port after exiting
  int i = 1;
  setsockopt(master.sd, SOL_SOCKET,SO_REUSEADDR,&i ,sizeof(int));
}

//type of socket created IPv4
void Connection::initialiseSocketType(int port){
  listenPort = port;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(listenPort);
}

void Connection::bindSocketToPort(){
  //bind the socket to given Port
  if (bind(master.sd, (struct sockaddr *)&address, sizeof(address))<0){
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  printf("Listener on port %d \n", listenPort);
}

//try to specify maximum of 10 pending connections for the master socket
void Connection::specifyMaxConMasterSocket(){
  if (listen(master.sd, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
}

void Connection::initProgramm(int port){
  createMasterSocket();
  initialiseSocketType(port);
  bindSocketToPort();
  specifyMaxConMasterSocket();
  //accept the incoming connection
  addrlen = sizeof(address);
  puts("Waiting for connections ...");
}
