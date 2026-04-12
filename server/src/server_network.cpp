#include "server_network.h"

#include <fstream>

#include "server_global.h"

#include <cryptopp/algparam.h>
#include <enet/enet.h>
#include <server_logger.h>
#include <yaml-cpp/yaml.h>

#include "server_utils.h"
#include "server_world.h"
#include "shared_crypto.h"
#include "shared_global.h"
#include "shared_network.h"
#include "shared_utils.h"

int enet_loop() {
  ENetEvent enet_event;

  while (enet_host_service(global::enet::enet_server, &enet_event, 0) > 0) {
    switch (enet_event.type) {
      case ENET_EVENT_TYPE_CONNECT: enet_event_connected(enet_event); break;
      case ENET_EVENT_TYPE_RECEIVE: enet_event_receive(enet_event); break;
      case ENET_EVENT_TYPE_DISCONNECT: enet_event_disconnected(enet_event); break;
      case ENET_EVENT_TYPE_NONE: break;
    }
  }
  return 0;
}

void enet_event_connected(const ENetEvent &enet_event) {
  log_info("player connected: " + enet_ip_to_string(enet_event.peer->address.host));

}

void enet_event_receive(const ENetEvent &enet_event) {
  using namespace global::enet;

  string pkt_data_string = shared::utils::packet_to_string(enet_event.packet);

  // initial packets (not encrypted)
  if (enet_event.channelID == 0) {
    switch (shared::network::get_pkt_type(pkt_data_string)) {
      case PKT_FROM_CLIENT_NAME_AND_UUID: {
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
          Player temp_player;
          temp_player.name = name; // nome dal pacchetto
          temp_player.uuid = uuid; // uuid dal pacchetto
          temp_player.server_private_key = shared::crypto::create_private_key();
          temp_player.server_public_key = shared::crypto::create_public_key(temp_player.server_private_key);
          temp_player.peer = enet_event.peer;

          string file_string = ("data/players/" + temp_player.uuid + ".yaml");
          std::ifstream file(file_string);

          if (!file.good()) {
            player_data::create_player_data_file(temp_player);
          }
          global::online_players.emplace(uuid, std::move(temp_player));
          global::peer_to_uuid.emplace(temp_player.peer, uuid);

          shared::network::send_packet(temp_player.peer,
            PKT_FROM_SERVER_PUBLIC_KEY,
            IntToString(temp_player.server_public_key),
            0,
            ENET_PACKET_FLAG_RELIABLE);
        }
        break;
      }

      case PKT_FROM_CLIENT_PUBLIC_KEY: {
        pkt_data_string.erase(0, pkt_data_string.find(']') + 1);
        const auto it = global::online_players.find(get_uuid_from_peer(enet_event));
        if (it == global::online_players.end()) {return;}
        Player* temp_player = &it->second;
        temp_player->client_public_key = Integer(pkt_data_string.c_str());
        temp_player->session_key = shared::crypto::calculate_session_key(temp_player->server_private_key, temp_player->client_public_key);
        temp_player->encryption_key = shared::crypto::create_encryption_key_from_session_key(temp_player->session_key);

        string file_string = ("data/players/" + temp_player->uuid + ".yaml");
        std::ifstream file(file_string);

        if (file.good()) {
          log_debug("player file found");
          YAML::Node player_file = YAML::LoadFile(file_string);
          if (player_file["password_hash"].as<string>().empty()) {
            log_debug("player still NOT registered");
          }
          else {
            log_debug("player already registered");
          }
          shared::network::send_packet(temp_player->peer, PKT_FROM_SERVER_ASK_PASSWORD, "", 1, 0);
        }
        break;
      }
    }
  }

  // not encrypted (coords, ecc)
  else if (enet_event.channelID == 1) {
    switch (shared::network::get_pkt_type(pkt_data_string)) {
      case PKT_FROM_CLIENT_COORDS: {
        pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

        Player* temp_player = get_player_from_uuid(get_uuid_from_peer(enet_event));

        size_t space1 = pkt_data_string.find(' ');
        size_t space2 = pkt_data_string.find(' ', space1 + 1);

        float loc_x = stof(pkt_data_string.substr(0, space1));
        float loc_y = stof(pkt_data_string.substr(space1 + 1, space2 - space1 - 1));
        float loc_z = stof(pkt_data_string.substr(space2 + 1));

        temp_player->location_x = loc_x;
        temp_player->location_y = loc_y;
        temp_player->location_z = loc_z;
        break;
      }
    }
  }

  // encrypted (password hash, chat messages, ecc)
  else if (enet_event.channelID == 2) {

    Player* temp_player = get_player_from_uuid(get_uuid_from_peer(enet_event));
    if (temp_player->encryption_key.empty()) return;
    string decrypted_string = shared::crypto::decrypt_string_with_key(pkt_data_string, temp_player->encryption_key);

    switch (shared::network::get_pkt_type(decrypted_string)) {
      case PKT_FROM_CLIENT_HASHED_PASSWORD: {
        using namespace YAML;
        using std::ofstream;

        string file_string = ("data/players/" + temp_player->uuid + ".yaml");
        std::ifstream file(file_string);

        if (!file.good()) {
          log_error("error");
          return;
        }
        log_debug("pass received");
        Node player_file = LoadFile(file_string);
        if (player_file["password_hash"].as<string>().empty()) {
          log_debug("saving register pass received");

          decrypted_string.erase(0, decrypted_string.find(']') + 1);
          if (!shared::utils::is_valid_hash(decrypted_string)) {
            log_error("hash not valid");
            enet_peer_disconnect_later(enet_event.peer, 0);
            return;
          }

          player_data::save_hashed_password(temp_player->uuid, decrypted_string);

          shared::network::send_packet(enet_event.peer, PKT_FROM_SERVER_CONFIRM_PASSWORD, "", 1, ENET_PACKET_FLAG_RELIABLE);

        }

        else {
          log_debug("checking login pass received");

          decrypted_string.erase(0, decrypted_string.find(']') + 1);
          if (!shared::utils::is_valid_hash(decrypted_string)) {
            log_error("hash not valid");
            enet_peer_disconnect_later(enet_event.peer, 0);
            return;
          }
          if (!player_data::is_hash_correct(temp_player->uuid, decrypted_string)) {
            log_debug("wrong password");
            enet_peer_disconnect_later(enet_event.peer, 0);
            return;
          }
          shared::network::send_packet(enet_event.peer, PKT_FROM_SERVER_CONFIRM_PASSWORD, "", 1, ENET_PACKET_FLAG_RELIABLE);
          temp_player->player_status = PLAYER_STATUS_AUTHENTICATED;

          send_player_list(temp_player->peer);

          send_a_player_has_connected(temp_player);
          log_debug("pass correct");

          // DEBUG
          std::string packet = world::create_chunk_string(world::get_chunk_key(0, 0));
          std::cout << packet << "\n";
          shared::network::send_packet(enet_event.peer, PKT_FROM_SERVER_CHUNK, packet, 1, ENET_PACKET_FLAG_RELIABLE);

          packet = world::create_chunk_string(world::get_chunk_key(-1, -1));
          std::cout << packet << "\n";
          shared::network::send_packet(enet_event.peer, PKT_FROM_SERVER_CHUNK, packet, 1, ENET_PACKET_FLAG_RELIABLE);
          //////////////////////
        }
        break;
      }
    }
  }
}

void enet_event_disconnected(const ENetEvent &enet_event) {
  log_info("player disconnected");
  string uuid = get_uuid_from_peer(enet_event);
  global::peer_to_uuid.erase(enet_event.peer);
  global::online_players.erase(uuid);
  send_a_player_has_disconnected(uuid);
}

int create_enet_host() {
  global::enet::address.port = global::config::port;
  global::enet::address.host = global::config::host;
  global::enet::enet_server = enet_host_create(&global::enet::address, global::config::max_players, 3, 0, 0);

  if (global::enet::enet_server == nullptr) {
    return 1;
  }

  return 0;
}

void send_players_location() {
  using std::to_string;

  string packet_string;
  bool first = true;

  // create string with coords of all online players
  for (const auto& temp_player : global::online_players) {
    if (temp_player.second.player_status == PLAYER_STATUS_AUTHENTICATED
      ) {
      if (!first) {
        packet_string += ";";
      }

      string uuid = temp_player.first;
      string str_x = to_string(temp_player.second.location_x);
      string str_y = to_string(temp_player.second.location_y);
      string str_z = to_string(temp_player.second.location_z);

      packet_string += uuid + " " + str_x.substr(0, str_x.find('.') + 3) + " " + str_y.substr(0, str_y.find('.') + 3) + " " + str_z.substr(0, str_z.find('.') + 3);

      first = false;
    }
  }

  // send the string to all players
  for (const auto& temp_player : global::online_players) {
    if (temp_player.second.player_status == PLAYER_STATUS_AUTHENTICATED) {
      shared::network::send_packet(temp_player.second.peer, PKT_FROM_SERVER_COORDS, packet_string, 1, 0);
    }
  }
}

void send_player_list(ENetPeer* peer) {
  string packet_string;
  bool first = true;

  // create string with all online player's uuids and names
  for (const auto& temp_loop_player : global::online_players) {
    if (!first) {
      packet_string += ";";
    }
    packet_string += temp_loop_player.first + " " + temp_loop_player.second.name;
    first = false;
  }

  shared::network::send_packet(peer, PKT_FROM_SERVER_PLAYER_LIST, packet_string, 1, ENET_PACKET_FLAG_RELIABLE);
  log_debug(packet_string);
}

void send_a_player_has_connected(Player* connected_player) {
  string packet_string = connected_player->uuid + " " + connected_player->name;

  for (const auto& temp_loop_player : global::online_players) {
      shared::network::send_packet(temp_loop_player.second.peer, PKT_FROM_SERVER_A_PLAYER_HAS_CONNECTED, packet_string, 1, ENET_PACKET_FLAG_RELIABLE);
  }
  log_debug(packet_string);
}

void send_a_player_has_disconnected(string uuid) {
  const string& packet_string = uuid;

  for (const auto& temp_loop_player : global::online_players) {
    shared::network::send_packet(temp_loop_player.second.peer, PKT_FROM_SERVER_A_PLAYER_HAS_DISCONNECTED, packet_string, 1, ENET_PACKET_FLAG_RELIABLE);
  }
  log_debug(packet_string);
}