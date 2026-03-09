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



int Reply(string input, int client_fd) {
	string res;
	if (input.find("PING") != string::npos) res = "+PONG\r\n";
	else if (input.find("ECHO") != string::npos) res = input.substr(5); 
  int status = send(client_fd, res.c_str(), res.size(), 0);
	return status;
}



void handle_connectoin(int client_fd) {
  vector<char> buf(1024);
  while (true) {
    fill(all(buf), 0);
    // returns number of bytes, takes the data from user as a pointer in the memory and the socket we will listen to
    size_t bytes_rcv = recv(client_fd, buf.data(), buf.size(), 0);


    // if -1 then it is an error
    if (bytes_rcv < 0) {
      cerr << "Error\n";
      continue;
    }
    // if 0 then only this socket will be diconnected
    if (bytes_rcv == 0) {
      cout << "Client disconnected on socket " << client_fd << endl;
      break; // This is the crucial part!
    }
    // else: send the response
    if (Reply(string(all(buf)), client_fd) < 0) {
			cerr << "Error\n";
			continue;
		}
  }
  close(client_fd);
}

