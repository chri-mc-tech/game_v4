#include "server_commands.h"

#include <string>
#include <unordered_set>

#include "server_logger.h"

enum command_permission {
  COMMAND_PERMISSION_USER = 0,
  COMMAND_PERMISSION_ADMIN = 1,
  COMMAND_PERMISSION_CONSOLE = 2
};

namespace commands {
  std::unordered_map<string, Command> command_list;

  void register_commands() {
    Command help;
    help.command = []() { cmd_help(); };
    help.permission = COMMAND_PERMISSION_USER;
    command_list["help"] = help;

    Command list;
    list.command = [] () { cmd_list(); };
    list.permission = COMMAND_PERMISSION_USER;
    command_list["list"] = list;


  }

  void process_command(const std::string &command) {
    command_list.find(command);

  }

  void cmd_help() {

  }

  void cmd_list() {

  }
}