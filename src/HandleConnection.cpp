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
#include "User.cpp"
#define all(a) a.begin(), a.end()
#define _pb pop_back
#define pf push_front
#define _pf pop_front
using namespace std;
typedef long long L;

Store store;

string send_(int client, string str) {
	send(client, str.c_str(), str.size(), 0);
	return str;
}

void print_(int client, string resp) {
	send_(client, resp);
}


queue<vector<string>> multi_q;
string Reply(int client, vector<string> input, User& user) {
	string res;
	deque<RespNode> res_arr;
	bool simple = false; // Determine if it is a simple string response
	bool null = false, null_arr = false;
	bool num = false;
	bool arr = false;
	bool err = false;
	if (user.getMulti()) user.setD(true);

	


	switch (StringCoding::command_string(input[0])) {
		// PING command
		case StringCoding::PING:
			res = "PONG";
			simple = true, num = null = false;
			break;
		// ECHO command
		case StringCoding::ECHO:
			if (input.size() == 1) return send_(client, Store::ERR(ERR::NUM_ARG, "echo"));
			res = "";
			for (int i = 1; i < input.size(); i++)
				res += input[i] + (i == input.size()-1 ? "":" ");
			simple = num = false;
			break;


		// SET command
		case StringCoding::SET:
			if (user.getMulti()) break;
			// Contains either 3 or 5 arguments only
			// SET Key Value (EX/PX) (Time To Live)
			if (input.size() < 3) return send_(client, Store::ERR(ERR::NUM_ARG, "set"));
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
			if (input.size() != 2) return send_(client, Store::ERR(ERR::NUM_ARG, "get"));
			
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
					} else return send_(client, Store::ERR(ERR::WRONG_T, ""));
				}
			
			} else res = "-1", null = true;
			
			break;



		// RPUSH command
		case StringCoding::RPUSH:
		case StringCoding::LPUSH: {
			if (user.getMulti()) break;
			// input should contain at least 3 arguments
			// RPUSH list_key appended_value/s
			deque<string> input_vec;
			string cm_name = StringCoding::command_code(StringCoding::command_string(input[0]));
			if (input.size() < 3) return send_(client, Store::ERR(ERR::NUM_ARG, cm_name));

			if (store.find(input[1]) != store.end()) {

				if (store.exp(input[1]) && Store::get_time() > store.exp(input[1]))
					store.erase(input[1]);
				else {
					for (int i = 2; i < input.size(); i++)
						input_vec.pb(input[i]);

					if (store.SET(input[1], {input_vec, 0}, cm_name == "lpush"))
						if (auto* vec = get_if<deque<string>>(&(store.GET(input[1]).second->val)))
							res = to_string(vec->size()), num = true;
				}

			} else {
				for (int i = 2; i < input.size(); i++)
					if (cm_name == "lpush") input_vec.pf(input[i]); 
					else input_vec.pb(input[i]);

				if (store.SET(input[1], {input_vec, 0}, cm_name == "lpush"))
					if (auto* vec = get_if<deque<string>>(&(store.GET(input[1]).second->val)))
						res = to_string(vec->size()), num = true;
			}

			break;
		}
			


		case StringCoding::LRANGE: {
			// input sould consist of 4 arguments
			// LRANGE list_key start end
			if (input.size() != 4) return send_(client, Store::ERR(ERR::NUM_ARG, "llrange"));
			pair<int, Entry*> p = store.GET(input[1]);
			int s, e;
			try {
				s = stoi(input[2]), e = stoi(input[3]);
			} catch (const exception& e) {
				break;
			}
			if (p.first) {
				if (auto* vec = get_if<deque<string>>(&(p.second->val))) {
					if (s < 0) s = max(0, (int)(vec->size()+s));
					if (e < 0) e = min((int)(vec->size()+e), (int)(vec->size())-1);
					if (s <= e && s < vec->size())
						for (int i = max(0, s); i <= min(e, (int)(vec->size())-1); i++)
							res_arr.pb(RespNode{(*vec)[i]});
				}
			}
			arr = true;
			break;
		}

		

		case StringCoding::LLEN:
			if (store.find(input[1]) == store.end() || !(get_if<deque<string>>(&(store.GET(input[1]).second->val)))) res = "0";
			else {

				auto* vec = get_if<deque<string>>(&(store.GET(input[1]).second->val));
				res = to_string(vec->size());

			}
			num = true;
			break;



		case StringCoding::LPOP:
			if (user.getMulti()) break;
			if (input.size() > 3 || input.size() < 2) return send_(client, Store::ERR(ERR::NUM_ARG, "lpop"));
			if (store.find(input[1]) == store.end()) null = true;
			if (auto* vec = get_if<deque<string>>(&(store.GET(input[1]).second->val))) {

				try {
					deque<RespNode> after_pop = store.ListRemove(input[1], (input.size() == 3 ? stoi(input[2]):1));
					if (after_pop.size() == 1) {
						if (auto* a_p = get_if<string>(&after_pop.front().val))
							res = (*a_p);
					} else res_arr = after_pop, arr = true;
				} catch (const exception& e) {}


			} else null = true;

			break;




		case StringCoding::BLPOP: {
			if (user.getMulti()) break;
			if (input.size() != 3) return send_(client, Store::ERR(ERR::NUM_ARG, "blpop"));
			auto start = chrono::steady_clock::now();
			bool found = false;
			double to;
			try {
				to = stod(input[2]);
			} catch (const exception& e) {}
			while (true) {
				
				if (store.find(input[1]) != store.end()) {
					if (auto* vec = get_if<deque<string>>(&(store.GET(input[1]).second->val))) {
						if (!vec->empty()) {
							res_arr.pb(RespNode{input[1]}), res_arr.pb(RespNode{vec->front()}), vec->_pf(), arr = found = true;
							break;
						}
					}
				}

				if (to) {
					auto now = chrono::steady_clock::now();
					chrono::duration<double> duration = now - start;
					if (duration.count() >= to) break;
				}
				this_thread::sleep_for(chrono::microseconds(100));
			}
			if (!found) null_arr = true;


			break;
		}


		
		case StringCoding::TYPE: {
			if (user.getMulti()) break;
			if (input.size() != 2) return send_(client, Store::ERR(ERR::NUM_ARG, "type"));
			auto data = store.GET(input[1]);
			if (data.second == nullptr)
				res = "none";
			else {
				auto& v = data.second->val;
				if (holds_alternative<string>(v)) res = "string";
				else if (holds_alternative<deque<string>>(v)) res = "list";
				else if (holds_alternative<unordered_map<string, string>>(v)) res = "hash";
				else if (holds_alternative<unordered_set<string>>(v)) res = "set";
				else if (holds_alternative<vector<Stream>>(v)) res = "stream";
			}
			simple = true;
			break;
		}



		case StringCoding::XADD: {
			if (user.getMulti()) break;
			if (input.size() < 5) return send_(client, Store::ERR(ERR::NUM_ARG, "xadd"));
			auto* vec = get_if<vector<Stream>>(&(store.GET(input[1]).second->val));
			if (store.find(input[1]) == store.end() || vec) {
				vector<Stream> stream;
				if (input[2].find("*") != string::npos)
					Stream::generate_id(input[2], vec != nullptr ? (*vec):stream);
				if (store.find(input[1]) != store.end() && !Stream::validate_id(input[2], *vec))
					res = "ERR The ID specified in XADD is equal or smaller than the target stream top item", err = true;
				if (input[2] == "0-0") res = "ERR The ID specified in XADD must be greater than 0-0", err = true;
				if (store.find(input[1]) == store.end() || Stream::validate_id(input[2], *vec)) {
					// vector<Stream> stream;
					Stream s; s.id = input[2];
					for (int i = 3; i < input.size(); i+=2)
						s.fields[input[i]] = input[i+1];
					stream.pb(s);
					store.SET(input[1], {stream, 0});
					res = input[2];
				}
			}
			break;
		}



		case StringCoding::XRANGE: {
			if (user.getMulti()) break;
        if (input.size() < 4) return send_(client, Store::ERR(ERR::NUM_ARG, "xrange"));

        auto data = store.GET(input[1]);
        if (data.second == nullptr) {
            // Key doesn't exist: Redis returns an empty array
            arr = true; 
            break;
        }

        // Use vector<Stream> to match your XADD implementation
        auto* vec = get_if<vector<Stream>>(&(data.second->val));
        if (!vec) return send_(client, Store::ERR(ERR::WRONG_T, "WRONGTYPE"));

        // 2. ID Normalization
        string startId = input[2];
        string endId = input[3];

        // Handle "-" and "+" special characters
        if (startId == "-") startId = "0-0";
        else if (startId.find('-') == string::npos) startId += "-0";

        if (endId == "+") endId = "9999999999999-9999999999999"; // Effective infinity
        else if (endId.find('-') == string::npos) endId += "-18446744073709551615"; // Max ULL

        // 3. Binary Search
        // Note: Redis Stream IDs (strings) compare correctly lexicographically 
        // as long as the timestamps have the same length or we use a custom comparator.
        auto startIt = lower_bound(vec->begin(), vec->end(), startId, 
            [](const Stream& s, const string& id) { return s.id < id; });
        
        auto endIt = upper_bound(vec->begin(), vec->end(), endId, 
            [](const string& id, const Stream& s) { return id < s.id; });

        // 4. Build the RESP response
        for (auto i = startIt; i != endIt; ++i) {
            deque<RespNode> entry;
            entry.push_back(RespNode{i->id}); // The ID of the entry
            
            deque<RespNode> fields;
            for (const auto& [key, val] : i->fields) {
                fields.push_back(RespNode{key});
                fields.push_back(RespNode{val});
            }
            entry.push_back(RespNode{fields}); // The fields/values array
            res_arr.push_back(RespNode{entry}); // Wrap entry in result array
        }
        
        arr = true;
        break;
    }




		case StringCoding::XREAD: {
			if (user.getMulti()) break;
			if (input.size() < 4) return send_(client, Store::ERR(ERR::NUM_ARG, "xread"));

			// 1. Parse optional BLOCK
			L timeout_ms = 0;
			bool block = false;
			int streams_kw_idx = -1;

			for (int i = 1; i < input.size(); ++i) {
				if (input[i] == "BLOCK" || input[i] == "block")
					block = true, timeout_ms = stoll(input[i + 1]);
				if (input[i] == "STREAMS" || input[i] == "streams") {
					streams_kw_idx = i;
					break;
				}
			}

			if (streams_kw_idx == -1) return "0"; // Should not happen with valid command


			if (streams_kw_idx == -1) return "0"; 

			int key_start = streams_kw_idx + 1;
			int num_keys = (input.size() - key_start) / 2;

			// 2. Resolve '$' IDs to the current top of the stream before blocking
			vector<string> resolved_ids;
			for (int i = 0; i < num_keys; i++) {
				string current_id = input[key_start + num_keys + i];
				if (current_id == "$") {
					auto data = store.GET(input[key_start + i]);
					if (data.second) {
						if (auto* v = get_if<vector<Stream>>(&(data.second->val))) {
							current_id = v->empty() ? "0-0" : v->back().id;
						}
					} else {
						current_id = "0-0";
					}
				}
				resolved_ids.push_back(current_id);
			}

			auto start_time = chrono::steady_clock::now();
			while (true) {
				res_arr.clear();
				for (int i = 0; i < num_keys; i++) {
					string current_key = input[key_start + i];
					string current_id = resolved_ids[i];

					auto data = store.GET(current_key);
					if (!data.second) continue;
					auto* vec = get_if<vector<Stream>>(&(data.second->val));
					if (!vec) continue;
					// XREAD retrieves IDs strictly greater than the specified ID
					auto it = upper_bound(vec->begin(), vec->end(), current_id, 
							[](const string& id, const Stream& s) { return id < s.id; });

					if (it != vec->end()) {
						deque<RespNode> entries_dq;
						for (; it != vec->end(); ++it) {
							deque<RespNode> entry;
							entry.push_back(RespNode{it->id});
							deque<RespNode> fields_flat;
							for (auto const& [k, v] : it->fields) {
								fields_flat.push_back(RespNode{k});
								fields_flat.push_back(RespNode{v});
							}
							entry.push_back(RespNode{fields_flat});
							entries_dq.push_back(RespNode{entry});
						}
						
						deque<RespNode> stream_dq;
						stream_dq.push_back(RespNode{current_key});
						stream_dq.push_back(RespNode{entries_dq});
						res_arr.push_back(RespNode{stream_dq});
					}
				}

				if (!res_arr.empty()) {
					arr = true;
					break;
				}

				if (!block) {
					null_arr = true;
					break;
				}

				// Handle Blocking Timeout
				auto now = chrono::steady_clock::now();
				auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - start_time).count();
				if (timeout_ms > 0 && elapsed >= timeout_ms) {
					null_arr = true;
					break;
				}
				this_thread::sleep_for(chrono::milliseconds(10));
			}
			break;
		}



		case StringCoding::INCR: {
			if (user.getMulti()) break;
			if (input.size() != 2) return send_(client, Store::ERR(ERR::NUM_ARG, "incr"));
			try {
				pair<int, Entry*> get_result = store.GET(input[1]);
				auto* data = get_if<string>(&get_result.second->val);

				if (!get_result.first)
					store.SET(input[1], Entry{"1", 0}), res = "1";
				else if (data)
					*data = to_string(stoll(*data)+1), res = *data;
				else
					throw exception();

				num = true;
				// if (!holds_alternative<string>(get_result.second->val))
			} catch (const exception& e) {
				return send_(client, RESP_Parser::make_simple_error("ERR value is not an integer or out of range"));
			}
			break;
		}


		case StringCoding::MULTI: {
			res = "OK", simple = true, user.setMulti(true);
			break;
		}

		case StringCoding::EXEC: {
			if (!user.getMulti()) return send_(client, RESP_Parser::make_simple_error("ERR EXEC without MULTI"));
			user.setMulti(false);
			string ret = "*" + to_string(multi_q.size()) + "\r\n";
			while (!multi_q.empty()) {
				ret += Reply(client, multi_q.front(), user), multi_q.pop();
				cout << ret << endl;
				
			}
			user.setD(false);
			return send_(client, ret);
			// arr = true;
			break;
		}

	}

	if (user.getD() and user.getMulti()) {
		multi_q.push(input);
		res = "QUEUED";
		return send_(client, RESP_Parser::make_simple_string(res));
	}


	string resp;
	cout << resp << '\n';
	if (!user.getD()) {
		if (null) // null string in RESP
			resp = "$-1\r\n";
		else if (null_arr)
			resp = "*-1\r\n";
		else if (err)
			resp = RESP_Parser::make_simple_error(res);
		else if (arr) {
			cout << "H";
			RespNode rn = {res_arr};
			resp = RESP_Parser::make_array(rn);
		}
		else if (num)
			resp = RESP_Parser::make_integer(res);
		else if (simple)
			resp = RESP_Parser::make_simple_string(res);
		else resp = RESP_Parser::make_bulk_string(res);
		
		print_(client, resp);
	} else {
		if (null) // null string in RESP
			resp = "$-1\r\n";
		else if (null_arr)
			resp = "*-1\r\n";
		else if (err)
			resp = RESP_Parser::make_simple_error(res);
		else if (arr) {
			cout << "H";
			RespNode rn = {res_arr};
			resp = RESP_Parser::make_array(rn);
		}
		else if (num)
			resp = RESP_Parser::make_integer(res);
		else if (simple)
			resp = RESP_Parser::make_simple_string(res);
		else resp = RESP_Parser::make_bulk_string(res);
	}
	return resp;
}



void handle_connectoin(int client_fd) {
  vector<char> buf(1024);
	User user;
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

    if (Reply(client_fd, command, user) == "") {
			cerr << "Error\n";
			continue;
		}
  }
  close(client_fd);
}

