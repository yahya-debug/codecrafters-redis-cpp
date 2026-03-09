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
#define pb push_back
using namespace std;
typedef long long L;

class RESP_Parser {
  public:
  static vector<string> parse_array(const string& inp) {
    vector<string> args;
    // if not array
    if (inp.empty() || inp[0] != '*')
      return args;
    size_t pos = 0;
    size_t next_rn = inp.find("\r\n", pos); // Search for the next \r\n
    // number of elements
    int number_of_elements = stoi(inp.substr(pos+1, next_rn-(pos+1)));
    pos = next_rn+2; // jump with the pos pointer ater \r\n (2 buffered inputs after the data)

    for (int i = 0; i < number_of_elements; i++) {
      if (inp[pos] != '$') break;

      // length of string
      next_rn = inp.find("\r\n", pos);
      int str_len = stoi(inp.substr(pos+1, next_rn-(pos+1)));
      pos = next_rn+2; // jump with the pos pointer ater \r\n (2 buffered inputs after the data)

      // Get the actual string data
      args.pb(inp.substr(pos, str_len));

      // jump with the pos pointer ater \r\n (2 buffered inputs after the data)
      pos = str_len+2;
    }
    return args;
  }


  // Wrap string back into RESP format to response
  static string make_bulk_string(const string& str) {
    return "$" + to_string(str.length()) + "\r\n" + str + "\r\n";
  }
  static string make_simple_string(const string& str) {
    return "+" + str + "\r\n";
  }
};