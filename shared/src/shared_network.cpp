#include "shared_network.h"

#include <enet/enet.h>
#include <cryptopp/integer.h>

#include "shared_global.h"

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

  bool send_packet(ENetPeer* peer, const int pkt_type, const string input_string, const int channel, const int flag) {
    string to_send = network::pkt_type(pkt_type) + input_string;
    ENetPacket *temp_packet = enet_packet_create(to_send.c_str(), to_send.length(), flag);
    if (enet_peer_send(peer, channel, temp_packet) == 0) {return true;}
    else {
      enet_packet_destroy(temp_packet);
      return false;
    }
  }
}