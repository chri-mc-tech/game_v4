#pragma once

#include <SDL3/SDL_rect.h>
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
  SDL_FRect* rect;
};

Player * get_player_from_uuid(const string& uuid);
