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
    for (int i = 0; i < t_string.length(); i++) {
      if (!std::isalnum(t_string[i]) && t_string[i] != '_') {
        std::cout << t_string[i] << std::endl;
        return false;
      }
    }
    return true;
  }

  bool is_valid_uuid(string t_string) {
    for (int i = 0; i < t_string.length(); i++) {
      if (!std::isalnum(t_string[i]) && t_string[i] != '-') {
        return false;
      }
        std::cout << i << ": " << t_string[i] << std::endl;
    }
    std::cout << t_string.length() << std::endl;
    if (t_string.length() != 29) {return false;}

    if (t_string[8] != '-') {return false;}
    if (t_string[17] != '-') {return false;}
    if (t_string[26] != '-') {return false;}

    return true;
  }

  string packet_to_string(const ENetPacket *packet) {
    string string(reinterpret_cast<char*>(packet->data), packet->dataLength);
    return string;
  }

}


