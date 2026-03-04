#include "server_commands.h"

#include <string>
#include <unordered_set>

#include "server_global.h"
#include "server_logger.h"

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

  void process_command(const std::string &command, int input_permission) {
    if (command_list.contains(command)) {
      command_list.find(command)->second.execute(input_permission);
    }
    else {
      log_error("command not found \"" + command + "\"");
    }

  }

  void cmd_help() {
    string temp;
    for (auto i : command_list) {
      if (temp.empty()) {
        temp = i.first;
      }
      else {
        temp += ", " + i.first;
      }
    }
    log_info(temp);
  }

  void cmd_list() {
    string temp;
    for (auto i : global::online_players) {
      if (temp.empty()) {
        temp = i.second.name;
      }
      else {
        temp += ", " + i.first;
      }
    }
    if (temp.empty()) {
      log_info("none");
    }
    else {
      log_info(temp);
    }
  }
}