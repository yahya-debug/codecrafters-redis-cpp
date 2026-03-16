#ifndef USER
#define USER
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
#include "Yahya.cpp"
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
  string role = "master";
  // L connected_slaves
  
  public:
  int ID;
  User(string role) {
    this->role = role;
  }
  User() {

  }
  queue<vector<string>> multi_q;

  string getRole() {
    return this->role;
  }

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

  

  virtual string getINFO(vector<string>& input) {
    return "";
  }
};


class Master : public User {
  private:
  string master_replid;
  int master_repl_offset;

  public:
  Master() : User("master") {
    master_replid = Yahya::Random(40);
    master_repl_offset = 0;
  }

  string getMaster_replid() {
    return this->master_replid;
  }
  int getMaster_repl_offset() {
    return this->master_repl_offset;
  }

  string getINFO(vector<string>& input) override {
    cout << "d";
    string res;
    if (input[1] == "replication") {
      res = "role:" + this->getRole() + "\r\n"
          + "master_replid:" + this->getMaster_replid() + "\r\n"
          + "master_repl_offset:" + to_string(this->getMaster_repl_offset());
    }
    return res;
  }
};
class Slave : public User {
  public:
  Slave() : User("slave") {

  }
  string getINFO(vector<string>& input) override {
    cout << "d";
    string res;
    if (input[1] == "replication") {
      res = "role:" + this->getRole() + "\r\n";
          // + "master_replid:" + this->getMaster_replid() + "\r\n"
          // + "master_repl_offset:" + to_string(this->getMaster_repl_offset());
    }
    return res;
  }
};

#endif