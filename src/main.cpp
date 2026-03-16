// #include "HandleConnection.hpp"
#include <iostream>
#include <cstdlib>
#include <string>
#include <thread>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <bits/stdc++.h>
#include "User.cpp"
using namespace std;
typedef long long L;

void handle_connectoin(User* user);

int main(int argc, char **argv) {
  // Flush after every cout / cerr
  cout << unitbuf;
  cerr << unitbuf;
  
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
   cerr << "Failed to create server socket\n";
   return 1;
  }
  
  // Since the tester restarts your program quite often, setting SO_REUSEADDR
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(6379);
  int port_val = 6379; // Store as local int first
  bool slave = false;

  string mh; int mp;
  for (int i = 1; i < argc; i++) {
    if (string(argv[i]) == "--port" && i + 1 < argc) {
      port_val = stoi(argv[++i]);
    }
    if (string(argv[i]) == "--replicaof") {
      slave = true;
      if (i+1 < argc)
        mh = Yahya::split(argv[i+1], " ")[0], mp = stoi(Yahya::split(argv[i+1], " ")[1]);
    }
  }
  server_addr.sin_port = htons(port_val); // Use htons() here!

  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
    // Correctly report the port in the error log
    cerr << "Failed to bind to port " << port_val << "\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    cerr << "listen failed\n";
    return 1;
  }
  
  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  cout << "Waiting for a client to connect...\n";
  
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  cout << "Logs from your program will appear here!\n";

  
	while (true) {
    User* user = nullptr;
  
    if (slave) {
      user = new Slave(mh, mp, port_val);
      // You might want to run this in a thread if you don't want it to block startup
      thread handshake_thread(&Slave::initiateHandshake, (Slave*)user);
      handshake_thread.detach();
    } else user = new Master();
		int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
		cout << "Client connected\n";
		if (client_fd < 0) continue;
    user->ID = client_fd;
		thread t(handle_connectoin, user);
		t.detach();
	}
	close(server_fd);

  return 0;
}
