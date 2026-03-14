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
    LPUSH,
    LRANGE,
    LLEN,
    LPOP,
    BLPOP,
    TYPE,
    XADD,
    XRANGE,
    XREAD,
    INCR,
    unknown
  };
  static StringCodes command_string(const string& str) {
    if (str == "PING") return PING;
    if (str == "ECHO") return ECHO;
    if (str == "SET") return SET;
    if (str == "GET") return GET;
    if (str == "RPUSH") return RPUSH;
    if (str == "LPUSH") return LPUSH;
    if (str == "LRANGE") return LRANGE;
    if (str == "LLEN") return LLEN;
    if (str == "LPOP") return LPOP;
    if (str == "BLPOP") return BLPOP;
    if (str == "TYPE") return TYPE;
    if (str == "XADD") return XADD;
    if (str == "XRANGE") return XRANGE;
    if (str == "XREAD") return XREAD;
    if (str == "INCR") return INCR;
    return unknown;
  }
  static string command_code(const StringCodes& code) {
    if (code == PING) return "ping";
    if (code == ECHO) return "echo";
    if (code == SET) return "set";
    if (code == GET) return "get";
    if (code == RPUSH) return "rpush";
    if (code == LPUSH) return "lpush";
    if (code == LRANGE) return "lrange";
    if (code == LLEN) return "llen";
    if (code == LPOP) return "lpop";
    if (code == BLPOP) return "blpop";
    if (code == TYPE) return "type";
    return "";
  }
};

