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
#include "RESP_Parser.cpp"
#define all(a) a.begin(), a.end()
using namespace std;
typedef long long L;



int Reply(vector<string> input, int client_fd) {
	string res;
	if (input[0] == "PING") res = "+PONG\r\n";
	else if (input[0] == "ECHO") {
		res = ""; 
		for (int i = 1; i < input.size(); i++)
			res += input[i] + (i == input.size()-1 ? "":" ");
	}
	// cout << RESP_Parser::make_bulk_string(res) << endl;
	string resp = RESP_Parser::make_bulk_string(res);
  int status = send(client_fd, resp.c_str(), resp.size(), 0);
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
		string raw_input(buf.begin(), buf.begin()+bytes_rcv);
		cout << raw_input;
		vector<string> command = RESP_Parser::parse_array(raw_input);
		// // cout << command[0] << '\n';
		if (command.empty()) continue;


    if (Reply(command, client_fd) < 0) {
			cerr << "Error\n";
			continue;
		}
  }
  close(client_fd);
}

