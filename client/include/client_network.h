#pragma once
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN

#include <enet/enet.h>
#include <iostream>
#include <string>

#include "shared_global.h"

using std::string;

int enet_loop();
int enet_event_connected(const ENetEvent &enet_event);
int enet_event_receive(const ENetEvent &enet_event);
int enet_event_disconnected(const ENetEvent &enet_event);
int create_enet_host();
int connect_to_server(const string& ip, const string& port = std::to_string(DEFAULT_PORT));
void wait_server_connection();
void send_location();