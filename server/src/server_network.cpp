#include "server_network.h"
#include "server_global.h"

#include <cryptopp/algparam.h>
#include <enet/enet.h>
#include <server_logger.h>

#include "shared_crypto.h"
#include "shared_global.h"
#include "shared_network.h"
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

void enet_event_connected() {
  log_info("player connected");

}

void enet_event_receive() {
  log_info("packet received");
  string pkt_data_string = shared::utils::packet_to_string(global::enet::enet_event.packet);

  log_debug("packet: " + pkt_data_string);

  // not encrypted
  if (global::enet::enet_event.channelID == 0) {

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_CLIENT_NAME_AND_UUID))) {

      // todo: controlla validità nome player e uuid, crea player, manda key pubblica
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      string name = pkt_data_string.substr(0, pkt_data_string.find(' '));
      string uuid = pkt_data_string.substr(pkt_data_string.find(' ') + 1);

      log_debug(name);
      if (shared::utils::is_valid_nickname(name)) {
        log_debug("name valid");
      }
      else {log_debug("name NOT valid"); return;}

      log_debug(uuid);
      if (shared::utils::is_valid_uuid(uuid)) {
        log_debug("uuid valid");
      }
      else {log_debug("uuid NOT valid"); return;}

      if (global::online_players.contains(uuid)) {
        log_warn("player with uuid " + uuid + " already online");
      }
      else {
        player temp_player;
        temp_player.name = name; // nome dal pacchetto
        temp_player.uuid = uuid; // uuid dal pacchetto
        temp_player.server_private_key = shared::crypto::create_private_key();
        temp_player.server_public_key = shared::crypto::create_public_key(temp_player.server_private_key);
        temp_player.peer = global::enet::enet_event.peer;

        global::online_players.emplace(uuid, std::move(temp_player));
        global::peer_to_uuid.emplace(temp_player.peer, uuid);

        string to_send = shared::network::pkt_type(PKT_FROM_SERVER_PUBLIC_KEY) + IntToString(temp_player.server_public_key);
        ENetPacket *temp_packet = enet_packet_create(to_send.c_str(), to_send.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(temp_player.peer, 0, temp_packet);
      }
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_CLIENT_PUBLIC_KEY))) {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);
      const auto it = global::online_players.find(get_uuid_from_peer());
      if (it == global::online_players.end()) {return;}
      player* temp_player = &it->second;
      temp_player->client_public_key = Integer(pkt_data_string.c_str());
      temp_player->session_key = shared::crypto::calculate_session_key(temp_player->server_private_key, temp_player->client_public_key);
      temp_player->encryption_key = shared::crypto::create_encryption_key_from_session_key(temp_player->session_key);
      log_debug("server public key " + IntToString(temp_player->server_public_key));
      log_debug("client public key " + IntToString(temp_player->client_public_key));
      log_debug("shared key " + IntToString(temp_player->session_key));
      // log_debug("encryption key " + (temp_player->encryption_key));

    }
  }

  // encrypted
  else if (global::enet::enet_event.channelID == 1) {

  }
}

void enet_event_disconnected() {
  log_info("player disconnected");
  string uuid = get_uuid_from_peer();
  global::peer_to_uuid.erase(global::enet::enet_event.peer);
  global::online_players.erase(uuid);

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

string get_uuid_from_peer() {
  auto peer_entry = global::peer_to_uuid.find(global::enet::enet_event.peer);
  if (peer_entry == global::peer_to_uuid.end())
    return "";

  return peer_entry->second;
}
