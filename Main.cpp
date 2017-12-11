//#include "Server.h"
#include <thread>
#include <iostream>
#include <string>
#include <unistd.h>
#include "Control.h"
#include "Connection.h"
#include <memory>


int main() {
  Connection connection1;

  Sockets socket1(connection1);
  Control control1(connection1,socket1);

  std::thread controlThread(&Control::controller, &control1);

  controlThread.join();
  std::cout << "controlthread safely ended\n";
  
  return 0;
}
