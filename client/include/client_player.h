#pragma once

#include <iostream>
#include <enet/enet.h>
#include <cryptopp/integer.h>

using std::string;
using CryptoPP::Integer;
using CryptoPP::SecByteBlock;

class player {
public:
  string uuid;
  string name;
  float location_x = 0.0f;
  float location_y = 0.0f;
};