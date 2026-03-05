  #include "server_commands.h"

#include <string>
#include <unordered_set>

#include "server_global.h"
#include "server_logger.h"
#include "server_utils.h"

void Command::execute(string args) const {
  /*
  if (input_permission < permission) {
    log_info("No permission");
    return;
  }
  */
  command(args);
}

namespace commands {
  std::unordered_map<string, Command> command_list;

  void register_commands() {
    Command help;
    help.command = cmd_help;
    command_list["help"] = help;

    Command list;
    list.command = cmd_list;
    command_list["list"] = list;

    Command kick;
    kick.command = cmd_kick;
    command_list["kick"] = kick;

  }

  void process_command(const std::string &command, const string& args) {
    if (command_list.contains(command)) {
      command_list.find(command)->second.execute(args);
    }
    else {
      log_error("command not found: \"" + command + "\"");
    }

  }

  void cmd_help(string args) {
    string temp;
    for (const auto& i : command_list) {
      if (temp.empty()) {
        temp = i.first;
      }
      else {
        temp += ", " + i.first;
      }
    }
    log_info(temp);
  }

  void cmd_list(string args) {
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

  void cmd_kick(string args) {
    if (args.empty()) {
      log_error("select a player");
    }
    else {
      Player* temp_player = get_player_from_name(args);
      if (temp_player) {
        enet_peer_disconnect_now(temp_player->peer, 0);
        global::peer_to_uuid.erase(temp_player->peer);
        global::online_players.erase(temp_player->uuid);
        log_info("player \"" + args + "\" kicked");
      }
      else {
        log_error("player \"" + args + "\" is not online");
      }

    }
  }
}