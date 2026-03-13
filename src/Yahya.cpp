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


class Yahya {
  public:
  static vector<string> split(const string& s, const string& delimiter) {
    vector<string> tokens;
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
  
    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
      token = s.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      tokens.push_back(token);
    }

    tokens.push_back(s.substr(pos_start)); // Add the last part
    return tokens;
  }
};