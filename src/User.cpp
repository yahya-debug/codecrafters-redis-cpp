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
#define _pb pop_back
#define pf push_front
#define _pf pop_front
using namespace std;
typedef long long L;


class User {
  private:
  bool multi = false;
  bool d = false;
  public:
  queue<vector<string>> multi_q;
  void setMulti(bool multi) {
    this->multi = multi;
  }
  bool getMulti() {
    return this->multi;
  }
  void setD(bool d) {
    this->d = d;
  }
  bool getD() {
    return this->d;
  }
};