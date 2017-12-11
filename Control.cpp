#include "Control.h"

Control::Control(Connection &connectIn, Sockets &socketIn):connection(connectIn),socket1(socketIn){
  message.type = MSG_TYPE;
}

void Control::sendMessage(){
  message.type = MSG_TYPE;
  if(userInput.size() > 0){
    int sendResult = send(sock,(char*)&message, sizeof(message), 0);
    if(sendResult < 0 ){
      perror("not sent");
    } else {
      memset(&message, 0, sizeof(message));
    }
  }
}

void Control::getInputForMessage(char name[]){
  //find name
  for(int i= 0; i < connection.sockets.size(); i++){
    if(strcmp(connection.sockets[i].name,name) == 0){
      //name found
      sock = connection.sockets[i].sd;
      std::cout << "You are now Chatting(312 Chars max.) with:\n " << name << " Type '-stop' to stop" << std::endl;
      break;
    } else {
      //name not found
      messaging == false;
      std::cout << name << " not in the list of Connections sorry\n ";
    }
  }
  //start messaging until "stop"
  while(messaging){
    std::cout << ">";
    getline(std::cin, userInput);
    std::copy(userInput.begin(), userInput.end(), message.data.text);
    if(userInput=="-stop"){
      messaging = false;
      std::cout << "Stopped Chatting with: " << name << std::endl;
      userInput="";
    } else {
      sendMessage();
    }
  }
}

void Control::controlSetup(){
  int con_port = 0;
  int con_ip;

  std::cout << "Welcome to P2P Chat!\n>";
  std::cout << "Please type in your Username(16 chars max):\n>";
  getline(std::cin,userInput );
  strcpy(connection.master.name, userInput.c_str());

  std::cout << "Please type in your listening Port(1024-65535):\n>";
  getline(std::cin, userInput);
  listenPort = std::stoi(userInput, nullptr);
  connection.initProgramm(listenPort);
  connection.master.port = listenPort;
  connection.master.hashCode = listenPort;

  std::cout << "Do you want to connect to another Peer? y or n\n";
  getline(std::cin, userInput);

  if(userInput == "y"){
    bool connected = false;
    char* ip = new char[20];

    while(!connected){
      std::cout << "Please enter an IP to connect to(ex.:192.0.0.1)\n";
      getline(std::cin, userInput);
      strcpy(ip, userInput.c_str());

      std::cout << "Please enter a Port to connect to(1024-65535)\n";
      getline(std::cin, userInput);
      con_port = std::stoi(userInput,nullptr);

      int cfd = connection.connect_socket(con_port,ip);
      if(cfd > 0){
        connected = true;
        struct clientSocket  newSocket;
        struct sockaddr_in addr;
        inet_aton(ip, (struct in_addr *)&addr.sin_addr.s_addr);

        // adding connection to sockets
        newSocket.sd = cfd;
        newSocket.ip = addr.sin_addr.s_addr;
        newSocket.port = con_port;
        connection.sockets.push_back(newSocket);

        // sending CON_MSG
        memset(&message, 0, sizeof(message));
        message.hashCode = connection.master.hashCode;
        message.data.connections[0].port = listenPort;
        strcpy(message.data.name, connection.master.name);
        message.type = CON_TYPE;
        int sendResult = send(cfd,(char*)&message, sizeof(message), 0);

      }else {
        std::cout << "Couldnt Connect to Server! Please Try again."  << std::endl;
      }
    }
    delete(ip);
  }
  peerThread = std::thread(&Sockets::programmThread,std::ref(socket1));
  memset(&message, 0, sizeof(message));
}


void Control::controller(){
  controlSetup();

  while(1){
    std::cout << "Enter '-help' for possible commands\n>";
    getline(std::cin, userInput);

    if(userInput == "-help"){
      std::cout << "'-help' - Shows possible Commands\n"
      <<"'message' - Send a Message to another Peer\n"
      <<"'q' - shuts down the Program";

    } else if(userInput == "message"){

      char partner[16];
      std::cout << "Following Peers are Online: \n>";
      for(int i= 0; i < connection.sockets.size(); i++){
        std::cout <<"Name: "<< connection.sockets[i].name<<" #"<< connection.sockets[i].hashCode<< std::endl;
      }

      std::cout << "To whom would you like to write?:\n>";
      getline(std::cin, userInput);
      strcpy(partner, userInput.c_str());
      messaging = true;
      getInputForMessage(partner);

    }else if(userInput == "q"){
      std::cout << "shutting down\n";
      peerThread.detach();
      return;
    } else {
      std::cout << "wrong input\n";
    }
  }
  peerThread.detach();
}
