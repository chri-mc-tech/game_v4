#pragma once

#include <enet/enet.h>
#include <iostream>

using std::string;

namespace shared::network {
  string from_packet_to_string(const ENetPacket *packet);
  string pkt_type(int int_type);
  bool send_packet(ENetPeer* peer, const int pkt_type, const string &input_string, const int channel, const int flag);

} // namespace shared::network
