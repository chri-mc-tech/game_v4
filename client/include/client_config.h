#pragma once
#include <iostream>

namespace config {
  bool check();
  void create_config_file();
  bool load_config();
  void save_new_nickname(const std::string& t_string);
  void add_config_comment(std::ofstream &file_out);
}