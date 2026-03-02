#pragma once

#include <enet/enet.h>
#include <iostream>

using std::string;

namespace shared::network {
  string from_packet_to_string(const ENetPacket *packet);
  string pkt_type(int int_type);

} // namespace shared::network
