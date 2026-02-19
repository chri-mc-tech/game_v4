#include "shared_network.h"

#include <enet/enet.h>
#include <iostream>

using std::string;

namespace shared::network {
  string from_packet_to_string(const ENetPacket *packet) {
    string string(reinterpret_cast<char *>(packet->data), packet->dataLength);
    return string;
  }

  int get_packet_type(ENetPacket t_packet) {
    string t_text = from_packet_to_string(&t_packet);

    t_text = t_text.substr(1, t_text.find("]"));
    t_text.erase(t_text.length() - 1, t_text.length());
    std::cout << t_text << std::endl;
    int type = stoi(t_text);
    if (!type) {
      return -1;
    }

    return type;
  }


} // namespace shared::network
