#include "server_console.h"

#include <iostream>

#include "server_commands.h"
#include "server_global.h"
#include "server_logger.h"

namespace console {
  // async function
  void console() {
    string input_string;
    while (global::running) {
      std::cout << ">> ";
      std::getline(std::cin, input_string);
      if (!input_string.empty()) {
        commands::process_command(input_string);
        input_string.clear();
      }
    }
  }
}

