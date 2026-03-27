#pragma once
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN

#include <cryptopp/integer.h>
#include <enet/enet.h>
#include <iostream>

using std::string;

class Player {
public:
  string uuid;
  string name;
  float location_x = 0;
  float location_y = 0;
};

Player * get_player_from_uuid(const string& uuid);
