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
  float location_z = 0.0f;
  ENetPeer* peer;
  Integer server_private_key;
  Integer server_public_key;
  Integer client_public_key;
  Integer session_key;
  SecByteBlock encryption_key;
};

namespace player_data {
  bool create_player_data_file(const Player& player);
  void save_hashed_password(const string& uuid, const string& hash);
  bool is_hash_correct(const string& uuid, const string& hash);

}