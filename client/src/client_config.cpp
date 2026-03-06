#include "../include/client_config.h"

#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

#include "client_global.h"
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
    config["uuid"] = shared::crypto::generate_uuid();
    config["name"] = "";
    config["show_fps"] = false;
    config["debug"] = false;

    ofstream file_out("config.yaml");

    add_config_comment(file_out);

    file_out << config;
    file_out.close();
    std::cout << "config file created" << std::endl;

  }

  bool load_config() {
    using namespace YAML;

    Node config = LoadFile("config.yaml");
    global::config::uuid = config["uuid"].as<string>();
    global::config::name = config["name"].as<string>();
    global::config::show_fps = config["show_fps"].as<bool>();
    global::config::debug = config["debug"].as<bool>();

    return true;
  }

  void save_new_nickname(const std::string& t_string) {
    using namespace YAML;
    using std::ofstream;

    Node config = LoadFile("config.yaml");
    config["name"] = t_string;
    ofstream file_out("config.yaml");

    add_config_comment(file_out);

    file_out << config;
    file_out.close();
    global::config::name = t_string;
  }

  void add_config_comment(std::ofstream &file_out) {
    using std::ofstream;
    file_out << "# Client configuration file\n";
    file_out << "# Player identity on servers is based on the UUID stored here\n";
    file_out << "# Changing the username will not change the UUID\n";
    file_out << "# Editing this file manually may cause data loss on servers\n";
    file_out << "# To generate a new UUID, delete this file and restart the client\n";
    file_out << "# WARNING: Doing so will lose access to previous server data\n\n";

  }
}