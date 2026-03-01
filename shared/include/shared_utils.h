#pragma once

#include <iostream>

using std::string;

namespace shared::utils {
  string get_current_time();
  bool is_valid_nickname(string t_string);
}
