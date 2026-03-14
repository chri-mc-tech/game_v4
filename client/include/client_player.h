#pragma once

#include <iostream>
#include <enet/enet.h>
#include <cryptopp/integer.h>

using std::string;

class Player {
public:
  string uuid;
  string name;
  float location_x = 0;
  float location_y = 0;
};