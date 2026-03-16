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
  private:
  string master_host;
  int master_port, listening_port;

  public:
  Slave(string host, int port, int l_port) : User("slave"), master_host(host), master_port(port), listening_port(l_port) {}

  void initiateHandshake() {
    int master_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in master_addr;
    master_addr.sin_family = AF_INET;
    master_addr.sin_port = htons(master_port);
    inet_pton(AF_INET, master_host.c_str(), &master_addr.sin_addr);

    if (connect(master_fd, (struct sockaddr*)&master_addr, sizeof(master_addr)) < 0) {
        cerr << "Failed to connect to master" << endl;
        return;
    }

    char buffer[1024];

    // Stage 1: PING
    string ping_cmd = "*1\r\n$4\r\nPING\r\n";
    send(master_fd, ping_cmd.c_str(), ping_cmd.size(), 0);
    recv(master_fd, buffer, sizeof(buffer), 0); // Wait for +PONG

    // Stage 2: REPLCONF listening-port
    string port_s = to_string(listening_port);
    string conf_port = "*3\r\n$8\r\nREPLCONF\r\n$14\r\nlistening-port\r\n$" 
                       + to_string(port_s.length()) + "\r\n" + port_s + "\r\n";
    send(master_fd, conf_port.c_str(), conf_port.size(), 0);
    recv(master_fd, buffer, sizeof(buffer), 0); // Wait for +OK

    // Stage 3: REPLCONF capa psync2
    string conf_capa = "*3\r\n$8\r\nREPLCONF\r\n$4\r\ncapa\r\n$6\r\npsync2\r\n";
    send(master_fd, conf_capa.c_str(), conf_capa.size(), 0);
    recv(master_fd, buffer, sizeof(buffer), 0); // Wait for +OK
    
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