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