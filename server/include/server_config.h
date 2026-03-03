#pragma once
#include <iosfwd>

namespace config {
  bool check();
  void create_config_file();
  bool load_config();
  void add_config_comment(std::ofstream &file_out);
}