#include "client_world.h"

#include <raylib.h>
#include "client_logger.h"

#include "client_textures.h"

namespace world {
  void save_chunk(const string& input_string) {
    Chunk chunk;
    string chunk_string = input_string.substr(0, input_string.find_first_of(';'));
    log_debug(chunk_string);
    chunk.x = stoi(chunk_string.substr(0, chunk_string.find(' ')));
    chunk.z = stoi(chunk_string.substr(chunk_string.find(' ') + 1));

    for (int x = 0; x < SIZE_X; ++x)
      for (int y = 0; y < SIZE_Y; ++y)
        for (int z = 0; z < SIZE_Z; ++z)
          chunk.blocks[x][y][z] = 0;

    string blocks_string = input_string.substr(input_string.find_first_of(';') + 1);

    while (blocks_string.find(';') != std::string::npos) {
      string block = blocks_string.substr(0, blocks_string.find_first_of(';'));
      blocks_string = blocks_string.substr(blocks_string.find_first_of(';') + 1);
      size_t p1 = block.find(',');
      size_t p2 = block.find(',', p1 + 1);
      size_t p3 = block.find(',', p2 + 1);

      if (p1 == string::npos || p2 == string::npos || p3 == string::npos) continue;

      int type = stoi(block.substr(0, p1));
      int x = stoi(block.substr(p1 + 1, p2 - p1 - 1));
      int y = stoi(block.substr(p2 + 1, p3 - p2 - 1));
      int z = stoi(block.substr(p3 + 1));

      chunk.blocks[x][y][z] = type;

    }
    chunks.emplace(chunk_string, chunk);
  }

  void render_chunk(const string& chunk_key) {
    auto it = chunks.find(chunk_key);
    if (it == chunks.end()) {
      log_debug("Chunk not found: " + chunk_key);
      return;
    }

    Chunk& chunk = it->second;

    int to_add_x = stoi(chunk_key.substr(0, chunk_key.find(' '))) * 16;
    int to_add_z = stoi(chunk_key.substr(chunk_key.find(' ') + 1)) * 16;

    for (int x = 0; x < SIZE_X; ++x) {
      for (int y = 0; y < SIZE_Y; ++y) {
        for (int z = 0; z < SIZE_Z; ++z) {
          int type = chunk.blocks[x][y][z];
          if (type <= 0) continue;

          std::string texture_name;
          if (type == 1) texture_name = "cobblestone";
          else if (type == 2) texture_name = "dirt";
          else if (type == 3) texture_name = "glass";
          else texture_name = "no_texture";

          textures::draw_block(
            (Vector3){(float)x + to_add_x + 0.5f, (float)y + 0.5f, (float)z + to_add_z + 0.5f},
            texture_name,
            WHITE
          );
        }
      }
    }
  }
}