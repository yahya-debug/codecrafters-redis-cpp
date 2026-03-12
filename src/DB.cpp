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
using namespace std;
typedef long long L;


class Stream {
  public:
  string id;
  unordered_map<string, string> fields;

  static bool validate_id(const string& str, vector<Stream>& this_) {
    if (str.find('-') == string::npos) return false;
    try {
      vector<string> tok = Yahya::split(str, "-");
      vector<string> prev;
      if (!this_.empty()) prev = Yahya::split(this_.back().id, "-");
      else prev = {"0", "0"};
      if (tok.size() != 2 || prev.size() != 2) return false;
      L ms = stoll(tok[0]), sn = stoll(tok[1]);
      if (stoll(prev[0]) > ms) return false;
      else if (stoll(prev[0]) == ms) {
        if (stoll(prev[1]) >= sn) return false;
      }
    } catch (const exception& e) {return false;}
    return true;
  }
};





struct Entry {
  variant<string, deque<string>, unordered_set<string>, unordered_map<string, string>, vector<Stream>> val;
	L exp;
};
enum class ERR {
  NUM_ARG,
  WRONG_T
};








class Store {
  public:
  
  unordered_map<string, Entry> store;
  static L get_time() {
    auto now = chrono::system_clock::now();
    return chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()).count();
  }

  int SET(const string& key, Entry value, bool front = false) {
    if (store.find(key) == store.end()) 
      store[key] = value;
    else if (holds_alternative<string>(store[key].val) && holds_alternative<string>(value.val))
      store[key] = value;
    else if (holds_alternative<deque<string>>(store[key].val) && holds_alternative<deque<string>>(value.val)) {
      if (auto* vec = get_if<deque<string>>(&(store[key].val)))
        if (auto* input_vec = get_if<deque<string>>(&(value.val)))
          for (string s:*input_vec)
            if (front) vec->push_front(s); 
            else vec->push_back(s);
    } else if (holds_alternative<vector<Stream>>(store[key].val) && holds_alternative<vector<Stream>>(value.val)) {
      if (auto* vec = get_if<vector<Stream>>(&(store[key].val))) {
        if (auto* input_vec = get_if<vector<Stream>>(&(value.val))) {
          for (Stream i:*input_vec)
            (*vec).push_back(i);
        }
      }
    } else return 0;
    return 1;
  }

  pair<int, Entry*> GET(const string& key) {
    if (store.find(key) == store.end())
      return {0, nullptr};
    return {1, &store[key]};
  }

  deque<string> ListRemove(const string& key, int count) {
    deque<string> ret;
    if (store.find(key) == store.end())
      return ret;
    if (auto* vec = get_if<deque<string>>(&(store[key].val)))
      for (int i = 0; i < count; i++)
        ret.push_back(vec->front()), vec->pop_front();
    return ret;
  }

  L exp(const string& key) {
    return store[key].exp;
  }

  unordered_map<string, Entry>::iterator end() {
    return store.end();
  }

  unordered_map<string, Entry>::iterator begin() {
    return store.begin();
  }

  unordered_map<string, Entry>::iterator find(const string& key) {
    return store.find(key);
  }

  size_t erase(const string& key) {
    return store.erase(key);
  }

  static string ERR(ERR err, const string& command) {
    switch (err) {
      case ERR::NUM_ARG:
        return "ERR wrong number of arguments for '" + command + "' command\n";
        break;
      case ERR::WRONG_T:
        return "WRONGTYPE Operation against a key holding the wrong kind of value\n";
    }
    return "";
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


