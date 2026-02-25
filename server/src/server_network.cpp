#include "server_network.h"
#include "server_global.h"

#include <server_logger.h>
#include <enet/enet.h>

#include "shared_crypto.h"

int enet_loop() {
  while (enet_host_service(global::enet::enet_server, &global::enet::enet_event, 0) > 0) {
    switch (global::enet::enet_event.type) {
      case ENET_EVENT_TYPE_CONNECT: enet_event_connected(); break;
      case ENET_EVENT_TYPE_RECEIVE: enet_event_receive(); break;
      case ENET_EVENT_TYPE_DISCONNECT: enet_event_disconnected(); break;
      case ENET_EVENT_TYPE_NONE: break;
    }
  }
  return 0;
}

int enet_event_connected() {
  log_info("player connected");
  return 0;
}

int enet_event_receive() {
  log_info("packet received");
  player temp_player;
  temp_player.private_key = shared::crypto::create_private_key();
  temp_player.server_public_key = shared::crypto::create_public_key(temp_player.private_key);
  return 0;
}

int enet_event_disconnected() {
  log_info("player disconnected");
  return 0;
}

int create_enet_host() {
  global::enet::address.port = config::port;
  global::enet::address.host = config::host;
  global::enet::enet_server = enet_host_create(&global::enet::address, config::max_players, 2, 0, 0);

  if (global::enet::enet_server == nullptr) {
    return 1;
  }

  return 0;
}