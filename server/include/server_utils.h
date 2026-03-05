#pragma once
#include <string>
#include "server_global.h"

std::string get_uuid_from_peer();
Player* get_player_from_name(const std::string& name);