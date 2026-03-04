#pragma once
#include <functional>
#include <iostream>
#include <string>

#include "server_logger.h"

namespace commands {
  void register_commands();
  void process_command(const std::string &command, int input_permission);

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

enum command_permission {
  COMMAND_PERMISSION_USER = 0,
  COMMAND_PERMISSION_ADMIN = 1,
  COMMAND_PERMISSION_CONSOLE = 2
};