#include "server_core.h"
#include "server_logger.h"
#include "shared_network.h"

#include <iostream>
#include <thread>

#include "server_config.h"
#include "server_console.h"
#include "server_global.h"
#include "server_network.h"

using namespace std;

int main() {
  if (!initialize_libraries()) {
    return 1;
  }

  if (!config::check()) {
    log_info("Config file not found, creating default...");
    config::create_config_file();
    if (!config::check()) {
      log_error("Error creating config file");
      return 4;
    }
  }
  else {
    log_info("Config file found, loading...");
  }

  if (!config::load_config()) {
    log_error("Error loading config file");
    return 5;
  }
  log_info("Server configuration loaded successfully");

  if (create_enet_host() != 0) {
    return 1;
  }

  /*
  log_info("info_test");
  log_warn("warn_test");
  log_error("error_test");
  */
  log_debug("Debug mode enabled");


  // log_debug(to_string(shared::network::get_packet_type("[1]ciao")));

  jthread thread_console(console::console);
  server_run();
}
