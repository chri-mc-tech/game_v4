#include "shared_network.h"

#include <enet/enet.h>
#include <cryptopp/integer.h>
#include <iostream>

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

  bool send_packet() { //todo: finisci la funzione
    string to_send = shared::network::pkt_type(PKT_FROM_SERVER_PUBLIC_KEY) + CryptoPP::IntToString(temp_player.server_public_key);
    ENetPacket *temp_packet = enet_packet_create(to_send.c_str(), to_send.length(), ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(temp_player.peer, 0, temp_packet);
  }

} // namespace shared::network
