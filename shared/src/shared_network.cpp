#include "shared_network.h"

#include <enet/enet.h>
#include <iostream>

using std::string;

namespace shared::network {
  string from_packet_to_string(const ENetPacket *packet) {
    string string(reinterpret_cast<char *>(packet->data), packet->dataLength);
    return string;
  }

  string pkt_type(int int_type) {
    string string_type = "[" + std::to_string(int_type) + "]";
    return string_type;
  }
} // namespace shared::network
