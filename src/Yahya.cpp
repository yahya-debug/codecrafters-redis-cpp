#ifndef YAHYA
#define YAHYA
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
  static string Random(size_t length) {
    // Define the character set to use for the ID
    const std::string characters =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    // Use std::random_device to obtain a seed for the random number engine
    // std::random_device is a non-deterministic source of randomness if available
    random_device random_device;

    // Seed the Mersenne Twister engine with the random device
    mt19937 generator(random_device());

    // Define a uniform integer distribution for the character set
    // The range is from 0 to the last index of the 'characters' string
    uniform_int_distribution<> distribution(0, characters.size() - 1);

    // Generate the string character by character
    string random_id;
    for (size_t i = 0; i < length; ++i) {
      random_id += characters[distribution(generator)];
    }

    return random_id;
  }
};

#endif