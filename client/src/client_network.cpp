#include "client_network.h"
#include "client_global.h"
#include "client_ui.h"

#include <thread>
#include <unistd.h>

#include "shared_crypto.h"
#include "shared_global.h"
#include "shared_network.h"
#include "shared_utils.h"

int create_enet_host() {
  global::enet::enet_client = enet_host_create(nullptr, 1, 2, 0, 0);

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
  std::cout << "connected" << std::endl;
  global::enet::is_connected = true;
  global::status = STATUS_ENCRYPTING;

  string to_send = shared::network::pkt_type(PKT_FROM_CLIENT_NAME_AND_UUID) + global::config::name + " " + global::config::uuid;
  ENetPacket *temp_packet = enet_packet_create(to_send.c_str(), to_send.length(), ENET_PACKET_FLAG_RELIABLE);
  enet_peer_send(global::enet::connected_server_peer, 0, temp_packet);
  std::cout << to_send << std::endl;
  return 0;
}

int enet_event_receive() {
  std::cout << "received" << std::endl;
  string pkt_data_string = shared::utils::packet_to_string(global::enet::enet_event.packet);

  std::cout << "packet: " + pkt_data_string << std::endl;

  // not encrypted
  if (global::enet::enet_event.channelID == 0) {

    if (pkt_data_string.starts_with(shared::network::pkt_type(PKT_FROM_SERVER_PUBLIC_KEY))) {
      pkt_data_string.erase(0, pkt_data_string.find(']') + 1);

      if (global::status == STATUS_ENCRYPTING) {
        global::client_private_key = shared::crypto::create_private_key();
        global::client_public_key = shared::crypto::create_public_key(global::client_private_key);
        global::server_public_key = Integer(pkt_data_string.c_str());
        global::shared_key = shared::crypto::calculate_session_key(global::client_private_key, global::server_public_key);
        global::encryption_key = shared::crypto::create_encryption_key_from_session_key(global::shared_key);
        std::cout << "server public key: " + IntToString(global::server_public_key) << std::endl;
        std::cout << "client public key: " + IntToString(global::client_public_key) << std::endl;
        std::cout << "shared key: " + IntToString(global::shared_key) << std::endl;
        std::cout << "hex encryption key: " + shared::crypto::secByteBlock_to_hex(global::encryption_key) << std::endl;

        shared::network::send_packet(
          global::enet::connected_server_peer,
          PKT_FROM_CLIENT_PUBLIC_KEY,
          IntToString(global::client_public_key),
          0, ENET_PACKET_FLAG_RELIABLE);

          /*
        string to_send = shared::network::pkt_type(PKT_FROM_CLIENT_PUBLIC_KEY) + IntToString(global::client_public_key);
        ENetPacket *temp_packet = enet_packet_create(to_send.c_str(), to_send.length(), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(global::enet::connected_server_peer, 0, temp_packet);
        */
      }
      /*
      todo: on packet receive tipo public key from server controlla tipo pacchetto e status.
      crea key privata, pubblica, e shared, manda pubblica

      global::client_private_key = shared::crypto::create_private_key();
      global::client_public_key = shared::crypto::create_public_key(global::client_private_key);
      */
    }
  }

  // encrypted
  else {

  }
  return 0;
}

int enet_event_disconnected() {
  std::cout << "disconnected" << std::endl;
  global::enet::is_connected = false;
  global::status = STATUS_DISCONNECTED_FROM_SERVER;

  return 0;
}

// return 0 if connected and 1 on error
int connect_to_server(const string& ip, const string& port) {
  global::status = STATUS_CONNECTING;

  ENetAddress server_to_connect;
  enet_address_set_host(&server_to_connect, ip.c_str());
  server_to_connect.port = static_cast<enet_uint16>(std::stoul(port));
  global::enet::connected_server_peer = enet_host_connect(global::enet::enet_client, &server_to_connect, 2, 0);
  std::jthread thread_wait_server_connection(wait_server_connection);

  thread_wait_server_connection.detach();
  return 0;
}

void wait_server_connection() {
  std::this_thread::sleep_for(std::chrono::seconds(6));
  if (!global::enet::is_connected) {
    global::status = STATUS_ERROR_CONNECTING_TO_SERVER;
    enet_peer_reset(global::enet::connected_server_peer);
  }
}