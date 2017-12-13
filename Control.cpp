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

void Control::getInputForMessage(uint32_t hashCode){
  //find name
  connection.mut.lock();
  for(int i= 0; i < connection.sockets.size(); i++){
    if(connection.sockets[i].hashCode== hashCode){
      //name found
      sock = connection.sockets[i].sd;
      std::cout << "You are now Chatting(312 Chars max.) with:\n " << connection.sockets[i].name << " #"<< connection.sockets[i].hashCode
      << std::endl<< " Type '-stop' to stop" << std::endl;
      connection.mut.unlock();
      break;
    } else {
      //name not found
      messaging == false;
      std::cout << hashCode << " not in the list of Connections sorry\n ";
    }
    connection.mut.unlock();
  }
  //start messaging until "stop"
  while(messaging){
    std::cout << ">";
    getline(std::cin, userInput);
    std::copy(userInput.begin(), userInput.end(), message.data.text);
    if(userInput=="-stop"){
      memset(&message, 0, sizeof(message));
      messaging = false;
      std::cout << "Stopped Chatting with: #" << hashCode << std::endl;
      userInput="";
    } else {
      sendMessage();
    }
  }
}

void Control::generateHash(){
  connection.master.hashCode= 0;
  srand(time(NULL));
  for(int i = 0; i < strlen(connection.master.name); i++ ){
    int randHash = rand() % 1000 + 1;
    connection.master.hashCode += (int)connection.master.name[i] + randHash;
  }
}

void Control::controlSetup(){
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
  generateHash();

  peerThread = std::thread(&Sockets::programmThread,std::ref(socket1));
  memset(&message, 0, sizeof(message));
}

void Control::sendConMsg(int sock,uint32_t hashCode, uint16_t port, char name[]){
  memset(&message, 0, sizeof(message));
  message.hashCode = hashCode;
  message.data.connections[0].port = port;
  strcpy(message.data.name, name);
  message.type = CON_TYPE;
  int sendResult = send(sock,(char*)&message, sizeof(message), 0);
}

void Control::controller(){
  controlSetup();

  while(1){
    std::cout<<"\n";
    std::cout << "Enter '-help' for possible commands\n>";
    getline(std::cin, userInput);

    if(userInput == "-help"){
      std::cout <<"-----------------------" << std::endl;
      std::cout <<"Possible Commands:" << std::endl;
      std::cout << "'-help' - Shows possible Commands\n"
      <<"'connect' - Connect to a Peer via IP and Port\n"
      <<"'message' - Send a Message to another Peer\n"
      <<"'q' - Shuts down the Program" << std::endl;
      std::cout <<"-----------------------" << std::endl;

    } else if(userInput == "message"){

      char partner[16];
      std::cout <<"-----------------------" << std::endl;
      std::cout << "Following Peers are Online: \n>";
      connection.mut.lock();
      for(int i= 0; i < connection.sockets.size(); i++){
        std::cout <<"Name: "<< connection.sockets[i].name<<" #"<< connection.sockets[i].hashCode<< std::endl;
      }
      connection.mut.unlock();
      std::cout <<"-----------------------" << std::endl;

      std::cout << "To whom would you like to write?(enter hashCode):\n>";
      getline(std::cin, userInput);
      int enteredHashCode = std::stoi(userInput, nullptr);
      messaging = true;
      getInputForMessage(enteredHashCode);

    } else if(userInput == "connect"){
      bool connected = false;
      int con_port = 0;
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
          connection.mut.lock();
          connection.sockets.push_back(newSocket);
          connection.mut.unlock();

          sendConMsg(cfd,connection.master.hashCode,listenPort,connection.master.name);

        }else {
          std::cout << "Couldnt Connect to Server! Please Try again."  << std::endl;
        }
        delete(ip);
      }

    }else if(userInput == "q"){
      std::cout << "shutting down\n";
      socket1.threadRunning = false;
      peerThread.join();
      std::cout << "socketThread safely ended\n";
      return;
    } else {
      std::cout << "wrong input\n";
    }
  }
}
