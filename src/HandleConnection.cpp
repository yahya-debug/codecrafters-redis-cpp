// // #include "HandleConnection.cpp"

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <bits/stdc++.h>
#define all(a) a.begin(), a.end()
using namespace std;
typedef long long L;

void handle_connectoin(int client_fd) {
  vector<char> buf(1024);
  string res = "+PONG\r\n";
  while (true) {
    fill(all(buf), 0);
    // returns number of bytes, takes the data from user as a pointer in the memory and the socket we will listen to
    size_t bytes_rcv = recv(client_fd, buf.data(), buf.size(), 0);
    // if -1 then it is an error
    if (bytes_rcv < 0) {
      cerr << "Error\n";
      continue;
    }
    
    if (send(client_fd, res.c_str(), res.size(), 0) < 0) {
      cerr << "Error\n";
      continue;
    }
  }
  close(client_fd);
}