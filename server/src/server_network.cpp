#include "server_network.h"
#include "server_global.h"

#include <server_logger.h>
#include <enet/enet.h>

#include "shared_crypto.h"
#include "shared_global.h"
#include "shared_utils.h"

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
  string pkt_data_string = shared::utils::packet_to_string(global::enet::enet_event.packet);

  // not encrypted
  if (global::enet::enet_event.channelID == 0) {

    if (pkt_data_string.starts_with(PKT_FROM_CLIENT_NAME_AND_UUID)) {
      // todo: controlla validita nome player e uuid, crea player, manda key pubblica
      pkt_data_string.erase(0, 3);

      string name = pkt_data_string.substr(0, pkt_data_string.find(' '));
      string uuid = pkt_data_string.substr(pkt_data_string.find(' ') + 1);

      std::cout << name << std::endl;
      std::cout << uuid << std::endl;
    }
  }

  // encrypted
  else if (global::enet::enet_event.channelID == 1) {

  }

  player temp_player;
  temp_player.name = ""; // nome dal pacchetto
  temp_player.uuid = ""; // uuid dal pacchetto
  temp_player.server_private_key = shared::crypto::create_private_key();
  temp_player.server_public_key = shared::crypto::create_public_key(temp_player.server_private_key);
  global::online_players.insert(uuid, temp_player);
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