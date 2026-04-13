#include "server_world.h"
#include "server_logger.h"

#include <string>
#include <fstream>
#include <filesystem>

using std::string;

namespace world {
  string get_chunk_key(int x, int z) {
    return std::to_string(x) + " " + std::to_string(z);
  }


  void load_chunk(int cx, int cz) {
    Chunk chunk;

    chunk.x = cx;
    chunk.z = cz;

    for (int x = 0; x < SIZE_X; x++)
      for (int y = 0; y < SIZE_Y; y++)
        for (int z = 0; z < SIZE_Z; z++)
          chunk.blocks[x][y][z] = 0;

    string file_name = "world/chunk_" + std::to_string(cx) + "_" + std::to_string(cz) + ".txt";
    std::ifstream file(file_name);

    if (file.is_open()) {
      int type, x, y, z;
      char comma;

      while (file >> type >> comma >> x >> comma >> y >> comma >> z) {
        chunk.blocks[x][y][z] = type;
      }
    }
    else {
      std::filesystem::create_directory("world");
      std::ofstream new_file(file_name);
      new_file.close();
    }

    std::string key = get_chunk_key(cx, cz);
    if (chunks.find(key) == chunks.end()) {
      chunks.emplace(key, chunk);
    }
  }


  void place_block(int type, int x, int y, int z) {
    int chunk_x = x / SIZE_X;
    int chunk_z = z / SIZE_Z;

    int local_x = x % SIZE_X;
    int local_y = y;
    int local_z = z % SIZE_Z;

    std::string key = get_chunk_key(chunk_x, chunk_z);

    if (chunks.find(key) == chunks.end()) {
      load_chunk(chunk_x, chunk_z);
    }

    chunks[key].blocks[local_x][local_y][local_z] = type;
  }

  string create_chunk_string(const string &chunk_key) {
    const Chunk &chunk = chunks[chunk_key];

    string data = chunk_key + ";";

    for (int x = 0; x < SIZE_X; x++)
      for (int y = 0; y < SIZE_Y; y++)
        for (int z = 0; z < SIZE_Z; z++)
          if (chunk.blocks[x][y][z] != 0)
            data += std::to_string(chunk.blocks[x][y][z]) + "," +
                    std::to_string(x) + "," +
                    std::to_string(y) + "," +
                    std::to_string(z) + ";";

    return data;
  }


}