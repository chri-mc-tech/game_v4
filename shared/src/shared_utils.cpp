#include "shared_utils.h"

#include <chrono>
#include <enet/enet.h>
#include <iostream>

using std::string;

namespace shared::utils {
  string get_current_time() {
    time_t t = time(nullptr);
    tm *now = localtime(&t);
    char buf[9];
    strftime(buf, sizeof(buf), "%H:%M:%S", now);
    return string(buf);
  }

  bool is_valid_nickname(string t_string) {
    if (t_string.length() < 4) {return false;}
    if (t_string.length() > 24) {return false;}
    for (int i = 0; i < t_string.length(); i++) {
      if (!std::isalnum(t_string[i]) && t_string[i] != '_') {
        return false;
      }
    }
    return true;
  }

  bool is_valid_uuid(string t_string) {
    if (t_string.length() != 29) {return false;}

    if (t_string[8] != '-') {return false;}
    if (t_string[17] != '-') {return false;}
    if (t_string[26] != '-') {return false;
    }

    string temp2 = t_string;
    temp2.erase(26, 1);
    temp2.erase(17, 1);
    temp2.erase(8, 1);

    for (const char c : temp2) {
      if (!std::isalnum(c)) {
        return false;
      }
      if (std::isalpha(c)) {
        if (!islower(c)) {
          return false;
        }
      }
    }
    return true;
  }

  bool is_valid_password(string t_string) {
    if (t_string.length() < 4) {return false;}
    for (int i = 0; i < t_string.length(); i++) {
      if (!std::isalnum(t_string[i]) && t_string[i] != '$' && t_string[i] != '@') {
        return false;
      }
    }
    return true;
  }

  bool is_valid_hash(const string hash) {
    if (hash.length() != 64) return false;

    for (char c : hash) {
      if (!std::isxdigit(c)) return false;
    }

    return true;
  }

  string packet_to_string(const ENetPacket *packet) {
    string string(reinterpret_cast<char*>(packet->data), packet->dataLength);
    return string;
  }

}


