#pragma once
#include <string>
#include "server_global.h"

string get_uuid_from_peer(const ENetEvent &enet_event);
Player* get_player_from_name(const string& name);
Player* get_player_from_uuid(const string& uuid);
string enet_ip_to_string(enet_uint32 ip);