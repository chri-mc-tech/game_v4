#pragma once
#include <string>
#include <unordered_map>

#include "shared_global.h"

class Chunk {
public:
  int x, z;

  int blocks[SIZE_X][SIZE_Y][SIZE_Z];
};

namespace world {
  inline std::unordered_map<std::string, Chunk> chunks;

  string get_chunk_key(int x, int z);
  void load_chunk(int cx, int cz);
  void place_block(int type, int x, int y, int z);
  string create_chunk_string(const string &chunk_key);
}