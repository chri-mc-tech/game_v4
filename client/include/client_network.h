#pragma once
#include <string>
#include <iostream>

#include "shared_global.h"

using std::string;

int enet_loop();
int enet_event_connected();
int enet_event_receive();
int enet_event_disconnected();
int create_enet_host();
int connect_to_server(const string& ip, const string& port = std::to_string(DEFAULT_PORT));
void wait_server_connection();
void send_location();