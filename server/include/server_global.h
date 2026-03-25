#pragma once

#include <fstream>


#include "server_player.h"
#include "shared_global.h"

#include <enet/enet.h>
#include <unordered_map>

namespace global {
  inline bool running;
  inline std::ofstream log_file;
  inline std::unordered_map<string, Player> online_players;
  inline std::unordered_map<ENetPeer*, string> peer_to_uuid;
}

namespace global::enet {
  inline ENetHost* enet_server = nullptr;
  inline ENetAddress address;
}

namespace global::config {
  inline bool debug;
  inline int port = DEFAULT_PORT;
  inline int host = 0;
  inline int max_players = 5;
}

enum player_status {
  PLAYER_STATUS_CONNECTED,
  PLAYER_STATUS_ENCRYPTING,
  PLAYER_STATUS_AUTHENTICATED,
};