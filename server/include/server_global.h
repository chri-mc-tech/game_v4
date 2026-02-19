#pragma once

#include "server_player.h"

#include <enet/enet.h>
#include <unordered_map>

namespace global {
  inline bool debug = true;
  inline bool running;
  inline std::unordered_map<string, player> online_players;
}

namespace global::enet {
  inline ENetEvent enet_event;
  inline ENetHost* enet_server = nullptr;
  inline ENetAddress address;
}

namespace config {
  inline int port = 23234;
  inline int host = 0;
  inline int max_players = 5;
}