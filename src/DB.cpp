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

struct Entry {
  string val;
	L exp;
};
class Store {
  public:
  static unordered_map<string, Entry> store;

  static L get_time() {
    auto now = chrono::system_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
  }
};

class ExpCode {
  public:
  enum Exp {
    NOT_SET,
    SECONDS,
    MSECOND
  };
  static Exp Exp_Ext(const string& str) {
    if (str == "EX") return SECONDS;
    if (str == "PX") return MSECOND;
    return NOT_SET;
  }
};


