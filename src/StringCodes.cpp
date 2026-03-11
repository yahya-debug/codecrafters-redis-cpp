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

class StringCoding {
  public:
  enum StringCodes {
    PING,
    ECHO,
    SET,
    GET,
    RPUSH,
    LRANGE,
    unknown
  };
  static StringCodes command_string(const string& str) {
    if (str == "PING") return PING;
    if (str == "ECHO") return ECHO;
    if (str == "SET") return SET;
    if (str == "GET") return GET;
    if (str == "RPUSH") return RPUSH;
    if (str == "LRANGE") return LRANGE;
    return unknown;
  }
};

