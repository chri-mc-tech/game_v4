#pragma once
#include <string>
#include "server_global.h"

std::string get_uuid_from_peer();
Player* get_player_from_name(const string& name);
Player* get_player_from_uuid(const string& uuid);
string enet_ip_to_string(enet_uint32 ip);