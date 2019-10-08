#include <iostream>
#include "HttpServer.h"

int main() {
  HttpServer Server(50000);
  Server.run();
  return 0;
}