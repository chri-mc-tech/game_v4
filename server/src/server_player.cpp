#include "server_player.h"

#include <fstream>
#include <server_logger.h>
#include <filesystem>

#include "server_global.h"

#include <yaml-cpp/yaml.h>

namespace player_data {
  bool create_player_data_file(const Player& player) {
    using namespace YAML;

    std::filesystem::create_directories("data/players");
    string file_path = "data/players/" + player.uuid + ".yaml";

    Node player_file;
    player_file["name"] = player.name;
    player_file["password_hash"] = "";

    std::ofstream fout(file_path);

    fout << player_file;
    fout.close();

    try {
      Node player_file_check = LoadFile(file_path);
    } catch (const BadFile&) {
      log_error("Error creating player file");
      return false;
    }

    return true;

  }

}