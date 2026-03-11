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
#include "DB.cpp"
#define all(a) a.begin(), a.end()
using namespace std;
typedef long long L;

Store store;


int Reply(vector<string> input, int client_fd) {
	string res;
	vector<string> res_arr;
	bool simple = false; // Determine if it is a simple string response
	bool null = false;
	bool num = false;
	bool arr = false;
	
	switch (StringCoding::command_string(input[0])) {
		// PING command
		case StringCoding::PING:
			res = "PONG";
			simple = true, num = null = false;
			break;
		// ECHO command
		case StringCoding::ECHO:
			if (input.size() == 1) return Store::ERR(ERR::NUM_ARG, "echo");
			res = "";
			for (int i = 1; i < input.size(); i++)
				res += input[i] + (i == input.size()-1 ? "":" ");
			simple = num = false;
			break;


		// SET command
		case StringCoding::SET:
			// Contains either 3 or 5 arguments only
			// SET Key Value (EX/PX) (Time To Live)
			if (input.size() < 3) return Store::ERR(ERR::NUM_ARG, "set");
			else if (input.size() == 3) {
				// by defult we use 0 to detect non-expiring data
				store.SET(input[1], {input[2], 0});
				res = "OK";
				simple = true;
			} else if (input.size() == 5) {
				L ttl;
				switch (ExpCode::Exp_Ext(input[3])) {
					case ExpCode::SECONDS:
						// EX: time to live in seconds so *1000 because our timing functoin uses milliseconds
						ttl = stoll(input[4])*1000;
						break;
					case ExpCode::MSECOND:
						// PX: time to live in milliseconds
						ttl = stoll(input[4]);
				}
				res = "OK", simple = true;
				store.SET(input[1], {input[2], Store::get_time()+ttl});
			} else cerr << "ERR syntax error\n";
			break;
		


		// GET command
		case StringCoding::GET:
			// input should contain only 2 arguments the command and the key
			// GET Key
			if (input.size() != 2) return Store::ERR(ERR::NUM_ARG, "get");
			
			// returns bulk string
			simple = false;
			if (store.find(input[1]) != store.end()) {
			
				// erase expired data
				if (store.exp(input[1]) && Store::get_time() > store.exp(input[1]))
					store.erase(input[1]), res = "-1", null = true;
				else {
					pair<int, Entry*> get_output = (store.GET(input[1]));
					if (get_output.first) {
						if (auto* str = get_if<string>(&(get_output.second)->val))
							res = *str;
					} else return Store::ERR(ERR::WRONG_T, "");
				}
			
			} else res = "-1", null = true;
			
			break;



		// RPUSH command
		case StringCoding::RPUSH:
			// input should contain at least 3 arguments
			// RPUSH list_key appended_value/s
			if (input.size() < 3) return Store::ERR(ERR::NUM_ARG, "rpush");

			if (store.find(input[1]) != store.end()) {

				if (store.exp(input[1]) && Store::get_time() > store.exp(input[1]))
					store.erase(input[1]);
				else {
					vector<string> input_vec;
					for (int i = 2; i < input.size(); i++)
						input_vec.pb(input[i]);

					if (store.SET(input[1], {input_vec, 0}))
						if (auto* vec = get_if<vector<string>>(&(store.GET(input[1]).second->val)))
							res = to_string(vec->size()), num = true;
				}

			} else {
				vector<string> input_vec;
				for (int i = 2; i < input.size(); i++)
					input_vec.pb(input[i]);

				if (store.SET(input[1], {input_vec, 0}))
					if (auto* vec = get_if<vector<string>>(&(store.GET(input[1]).second->val)))
						res = to_string(vec->size()), num = true;
			}

			break;
			


		case StringCoding::LRANGE:
			// input sould consist of 4 arguments
			// LRANGE list_key start end
			pair<int, Entry*> p = store.GET(input[1]);
			int s, e;
			try {
				s = stoi(input[2]), e = stoi(input[3]);
			} catch (const exception& e) {
				break;
			}
			if (p.first) {
				if (auto* vec = get_if<vector<string>>(&(p.second->val))) {
					if (s < 0) s = max(0, (int)(vec->size()+s));
					if (e < 0) e = min((int)vec->size()+e, (int)(vec->size())-1);
					if (s <= e && s < vec->size())
						for (int i = max(0, s); i <= min(e, (int)(vec->size())-1); i++)
							res_arr.pb(vec->at(i));
				}
			}
			arr = true;
			break;


	}



	string resp;
	if (null) // null string in RESP
		resp = "$-1\r\n";
	else if (arr)
		resp = RESP_Parser::make_array(res_arr);
	else if (num)
		resp = RESP_Parser::make_integer(res);
	else if (simple)
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

