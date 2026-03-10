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
#include "StringCodes.cpp"
#define all(a) a.begin(), a.end()
using namespace std;
typedef long long L;


unordered_map<string, string> store;

int Reply(vector<string> input, int client_fd) {
	string res;
	bool simple = false; // Determine if it is a simple string response

	switch (StringCoding::command_string(input[0])) {
		case StringCoding::PING:
			res = "PONG";
			simple = true;
			break;
		case StringCoding::ECHO:
			res = "";
			for (int i = 1; i < input.size(); i++)
				res += input[i] + (i == input.size()-1 ? "":" ");
			simple = false;
			break;
		case StringCoding::SET:
			if (input.size() < 3) cerr << "ERR wrong number of arguments for 'set' command\n";
			else if (input.size() == 3) {
				store[input[1]] = input[2];
				res = "OK";
				simple = true;
			} else cerr << "ERR syntax error\n";
			break;
		case StringCoding::GET:
			if (input.size() < 2) cerr << "ERR wrong number of arguments for 'set' command\n";
			else if (input.size() == 2) {
				res = store[input[1]];
				simple = false;
			} else cerr << "ERR syntax error\n";
			break;
	}

	string resp;
	if (simple)
		resp = RESP_Parser::make_simple_string(res);
	else resp = RESP_Parser::make_bulk_string(res);
	return send(client_fd, resp.c_str(), resp.size(), 0);
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
		vector<string> command = RESP_Parser::parse_array(raw_input);
		if (command.empty()) continue;


    if (Reply(command, client_fd) < 0) {
			cerr << "Error\n";
			continue;
		}
  }
  close(client_fd);
}

