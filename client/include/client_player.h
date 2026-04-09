#pragma once
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN

#include <cryptopp/integer.h>
#include <enet/enet.h>
#include <iostream>
#include <raylib.h>

using std::string;

class Player {
public:
  string uuid;
  string name;
  Vector3 location;
  int block_x;
  int block_y;
  int block_z;
  int chunk_x;
  int chunk_z;
};

Player * get_player_from_uuid(const string& uuid);
