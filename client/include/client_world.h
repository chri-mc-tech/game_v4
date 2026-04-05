#pragma once

#include "shared_global.h"

class Chunk {
public:
  int x, z;

  int blocks[SIZE_X][SIZE_Y][SIZE_Z];
};

namespace world {
  inline std::unordered_map<string, Chunk> chunks;
  void render_chunk(const string& input_string);
}