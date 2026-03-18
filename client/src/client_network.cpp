#include "client_network.h"
#include "client_global.h"
#include "client_ui.h"

#include <thread>
#include <unistd.h>

#include "client_logger.h"
#include "shared_crypto.h"
#include "shared_global.h"
#include "shared_network.h"
#include "shared_utils.h"

int create_enet_host() {
  global::enet::enet_client = enet_host_create(nullptr, 1, 3, 0, 0);

  if (global::enet::enet_client == nullptr) {
    return 1;
  }

  return 0;
}

int enet_loop() {
  while (enet_host_service(global::enet::enet_client, &global::enet::enet_event, 0) > 0) {
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
  log_debug("connected");
  global::enet::is_connected = true;
  global::status_connection = STATUS_CONNECTION_ENCRYPTING;

  string to_send = shared::network::pkt_type(PKT_FROM_CLIENT_NAME_AND_UUID) + global::config::name + " " + global::config::uuid;
  ENetPacket *temp_packet = enet_packet_create(to_send.c_str(), to_send.length(), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(global::enet::connected_server_peer, 0, temp_packet);
  return 0;
}

int enet_event_receive() {
  log_debug("received");
  string pkt_data_string = shared::utils::packet_to_string(global::enet::enet_event.packet);

  // initial packets (not encrypted)
  if (global::enet::enet_event.channelID == 0) {
    log_debug(pkt_data_string.substr(0, pkt_data_string.find(']') + 1));

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_PUBLIC_KEY))) {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      if (global::status_connection == STATUS_CONNECTION_ENCRYPTING) {
        global::client_private_key = shared::crypto::create_private_key();
        global::client_public_key = shared::crypto::create_public_key(global::client_private_key);
        global::server_public_key = Integer(pkt_data_string.c_str());
        global::shared_key = shared::crypto::calculate_session_key(global::client_private_key, global::server_public_key);
        global::encryption_key = shared::crypto::create_encryption_key_from_session_key(global::shared_key);

        shared::network::send_packet(
          global::enet::connected_server_peer,
          PKT_FROM_CLIENT_PUBLIC_KEY,
          IntToString(global::client_public_key),
          0,
          ENET_PACKET_FLAG_RELIABLE);

      }
    }
  }

  // not encrypted (coords, ecc)
  else if (global::enet::enet_event.channelID == 1) {
    log_debug(pkt_data_string.substr(0, pkt_data_string.find(']') + 1));

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_ASK_REGISTER_PASSWORD))) {
      global::status_menu = STATUS_MENU_WAITING_USER_INPUT_REGISTER_PASSWORD;
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_ASK_LOGIN_PASSWORD))) {
      global::status_menu = STATUS_MENU_WAITING_USER_INPUT_LOGIN_PASSWORD;
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_CONFIRM_REGISTER_PASSWORD))) {
      //global::status = STATUS_AUTHENTICATED;
      global::status_menu = STATUS_MENU_CONNECTED;
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_CONFIRM_LOGIN_PASSWORD))) {
      //global::status = STATUS_AUTHENTICATED;
      global::status_menu = STATUS_MENU_CONNECTED;

    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_DENY_REGISTER_PASSWORD))) {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);
      log_warn(pkt_data_string);
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_DENY_LOGIN_PASSWORD))) {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);
      log_warn(pkt_data_string);
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_PLAYER_LIST))) {
      // todo: crea player object per ogni player e aggiungi a online players

      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      size_t start = 0;

      while (start < pkt_data_string.size()) {
        size_t end = pkt_data_string.find(';', start);

        if (end == std::string::npos)
          end = pkt_data_string.size();
        string player_object = pkt_data_string.substr(start, end - start);

        size_t space = player_object.find(' ');

        string uuid = player_object.substr(0, space);
        string name = player_object.substr(space + 1);
        // log_debug(uuid + "|" + name);
        if (uuid != global::config::uuid) {
          Player temp_player;
          temp_player.name = name;
          temp_player.uuid = uuid;

          global::online_players.emplace(uuid, std::move(temp_player));
        }
        start = end + 1;
      }
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_A_PLAYER_HAS_CONNECTED))) {
      // todo: crea player object per player collegato e aggiungi a online players

      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      size_t space = pkt_data_string.find(' ');

      string uuid = pkt_data_string.substr(0, space);
      string name = pkt_data_string.substr(space + 1);
      // log_debug(uuid + "|" + name);
      if (uuid != global::config::uuid) {
        Player temp_player;
        temp_player.uuid = uuid;
        temp_player.name = name;

        global::online_players.emplace(uuid, std::move(temp_player));
      }
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_A_PLAYER_HAS_DISCONNECTED))) {
      // todo: togli player da online players
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      global::online_players.erase(pkt_data_string);
    }

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_COORDS))) {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      size_t start = 0;

      while (start < pkt_data_string.size()) {
        size_t end = pkt_data_string.find(';', start);

        if (end == string::npos)
          end = pkt_data_string.size();
        string player_object = pkt_data_string.substr(start, end - start);

        size_t space1 = player_object.find_first_of(' ');
        size_t space2 = player_object.find_last_of(' ');

        string uuid = player_object.substr(0, space1);
        string x = player_object.substr(space1 + 1, space2 - space1 - 1);
        string y = player_object.substr(space2 + 1);
        if (uuid != global::config::uuid) {

          Player* temp_player = get_player_from_uuid(uuid);
          temp_player->location_x = stoi(x);
          temp_player->location_y = stoi(y);
        }
        start = end + 1;

      }
    }

  }

  // encrypted (chat messages, ecc)
  else if (global::enet::enet_event.channelID == 2) {
    if (!global::encryption_key.empty()) {
      string decrypted_string = shared::crypto::decrypt_string_with_key(pkt_data_string, global::encryption_key);
      log_debug(decrypted_string.substr(0, decrypted_string.find(']') + 1));
    }
  }
  return 0;
}

int enet_event_disconnected() {
  log_debug("disconnected");
  global::enet::is_connected = false;
  global::status_connection = STATUS_CONNECTION_NOT_CONNECTED;
  global::status_menu = STATUS_MENU_DISCONNECTED_FROM_SERVER;

  return 0;
}

int connect_to_server(const string& ip, const string& port) {
  global::status_connection = STATUS_CONNECTION_CONNECTING;

  ENetAddress server_to_connect;
  enet_address_set_host(&server_to_connect, ip.c_str());
  server_to_connect.port = static_cast<enet_uint16>(std::stoul(port));
  global::enet::connected_server_peer = enet_host_connect(global::enet::enet_client, &server_to_connect, 3, 0);

  std::jthread thread_wait_server_connection(wait_server_connection);
  thread_wait_server_connection.detach();
  return 0;
}

// async function
void wait_server_connection() {
  std::this_thread::sleep_for(std::chrono::seconds(6));
  if (!global::enet::is_connected) {
    if (global::status_connection == STATUS_CONNECTION_CONNECTING) {
      global::status_menu = STATUS_MENU_DISCONNECTED_FROM_SERVER;
      global::status_connection = STATUS_CONNECTION_NOT_CONNECTED;
      enet_peer_reset(global::enet::connected_server_peer);
    }
  }
}

// async function
void every_second_log_debug(const string& text) {

  std::this_thread::sleep_for(std::chrono::seconds(1));
}


void send_location() {
  if (global::enet::is_connected) {
    shared::network::send_packet(
      global::enet::connected_server_peer,
      PKT_FROM_CLIENT_COORDS,
      (std::to_string(global::main_player.location_x) + " " + std::to_string(global::main_player.location_y)),
      1, 0);
  }
}