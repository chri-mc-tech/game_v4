#include "server_config.h"

#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include "shared_global.h"
#include "server_global.h"
#include "server_logger.h"
#include "shared_crypto.h"

namespace config {
  bool check() {
    using namespace YAML;

    try {
      Node config = LoadFile("config.yaml");
    } catch (const BadFile&) {
      return false;
    }
    return true;
  }

  void create_config_file() {
    using namespace YAML;
    using std::ofstream;

    Node config;
    config["debug"] = false;
    config["port"] = static_cast<int>(DEFAULT_PORT);
    config["max_players"] = 5;
    ofstream file_out("config.yaml");

    add_config_comment(file_out);

    file_out << config;
    file_out.close();
    log_info("config file created");

  }

  bool load_config() {
    using namespace YAML;

    Node config = LoadFile("config.yaml");
    global::config::debug = config["debug"].as<bool>();
    global::config::port = config["port"].as<int>();
    global::config::max_players = config["max_players"].as<int>();

    return true;
  }

  void add_config_comment(std::ofstream &file_out) {
    using std::ofstream;
    file_out << "# Configuration file\n\n";

  }
}