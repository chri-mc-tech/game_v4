#pragma once
#include <functional>
#include <iostream>
#include <string>

#include "server_logger.h"

namespace commands {
  void process_command(const std::string &command);

  void cmd_help();
  void cmd_list();
}

class Command {
public:
  std::function<void()> command;
  int permission;

  void execute(int input_permission) const {
    if (input_permission < permission) {
      log_info("No permission");
      return;
    }
    command();
  }
};