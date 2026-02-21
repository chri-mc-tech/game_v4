#include "client_network.h"
#include "client_global.h"
#include "ui.h"

#include <unistd.h>
#include <thread>

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

  return 0;
}

int enet_event_receive() {
  std::cout << "received" << std::endl;
  return 0;
}

int enet_event_disconnected() {
  std::cout << "disconnected" << std::endl;
  global::enet::is_connected = false;
  return 0;
}

// return 0 if connected and 1 on error
int connect_to_server(const string& ip, const string& port) {
  global::status = STATUS_WAITING_TO_CONNECT;

  global::ttf::loading_screen_text = "connecting";

  ENetAddress server_to_connect;
  enet_address_set_host(&server_to_connect, ip.c_str());
  server_to_connect.port = static_cast<enet_uint16>(std::stoul(port));
  global::enet::connected_server_peer = enet_host_connect(global::enet::enet_client, &server_to_connect, 2, 0);
  std::jthread wait_timeout(wait_6_seconds_timeout);

  wait_timeout.detach();
  return 0;
}

// TODO
// ciao me del futuro,
// non so neanche io a che punto sono arrivato
// quindi rileggiti il codice e capisci da solo che cazzo devi fare,
// io c'ho pure mal di pancia e vado a dormire, cazzi tuoi mo

void wait_6_seconds_timeout() {
  std::this_thread::sleep_for(std::chrono::seconds(6));
  if (!global::enet::is_connected) {
    global::status = STATUS_ERROR_CONNECTING_TO_SERVER;
  }
}