#include "client_network.h"
#include "client_global.h"

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
  return 0;
}

int enet_event_receive() {
  return 0;
}

int enet_event_disconnected() {
  return 0;
}