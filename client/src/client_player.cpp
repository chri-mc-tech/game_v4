#include "client_player.h"

#include "client_global.h"

Player * get_player_from_uuid(const string& uuid) {
  const auto it = global::online_players.find(uuid);
  if (it != global::online_players.end()) {
    return &it->second;
  }
  return nullptr;
}