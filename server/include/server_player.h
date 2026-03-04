#pragma once

#include <enet/enet.h>
#include <cryptopp/integer.h>

using std::string;
using CryptoPP::Integer;
using CryptoPP::SecByteBlock;

class Player {
public:
  string uuid;
  string name;
  int player_status = 0;
  float location_x = 0.0f;
  float location_y = 0.0f;
  ENetPeer* peer;
  Integer server_private_key;
  Integer server_public_key;
  Integer client_public_key;
  Integer session_key;
  SecByteBlock encryption_key;
};