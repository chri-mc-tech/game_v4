#include "server_world.h"

#include <string>
#include <fstream>
#include <filesystem>

void save_block(int id, int x, int y, int z, int chunk_x, int chunk_z) {
  std::filesystem::create_directory("world");
  std::string file_name = "world/chunk_" + std::to_string(chunk_x) + "-" + std::to_string(chunk_z) + ".txt";

  /*
  std::ofstream file(file_name, std::ios::app);

  if (file.is_open()) {
    file << id << "," << x << "," << y << "," << z << "\n";
    file.close();
  }
  */
}