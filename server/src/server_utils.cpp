#include "server_utils.h"

#include "server_global.h"

string get_uuid_from_peer() {
  auto peer_entry = global::peer_to_uuid.find(global::enet::enet_event.peer);
  if (peer_entry == global::peer_to_uuid.end())
    return "";

  return peer_entry->second;
}

Player* get_player_from_name(const string& name) {
  for (auto& temp : global::online_players) {
    if (temp.second.name == name) {
      return &temp.second;
    }
  }
  return nullptr;
}

string enet_ip_to_string(enet_uint32 ip) {
  uint8_t b1 = (ip >> 24) & 0xFF;
  uint8_t b2 = (ip >> 16) & 0xFF;
  uint8_t b3 = (ip >> 8) & 0xFF;
  uint8_t b4 = (ip) & 0xFF;

  return std::to_string(b4) + "." + std::to_string(b3) + "." +
         std::to_string(b2) + "." + std::to_string(b1);
}