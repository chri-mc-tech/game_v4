#include "client_core.h"
#include "client_network.h"

#include <iostream>

#include "client_config.h"
#include "client_global.h"
#include "client_logger.h"
#include "shared_crypto.h"
#include "shared_utils.h"

using std::endl;

int main() {
  if (!initialize_libraries()) {
    return 3;
  }

  if (!config::check()) {
    log_info("no config file found");
    config::create_config_file();
    if (!config::check()) {
      return 4;
    }
  }

  if (!config::load_config()) {
    return 5;
  }


  if (!shared::utils::is_valid_uuid(global::config::uuid)) {
    return 8;
  }

  log_debug(global::config::uuid);
  log_debug(global::config::name);

  if (create_enet_host() != 0) {
    return 6;
  }

  // log_debug(to_string(shared::network::get_packet_type("[1]ciao")));

  if (global::config::debug_console) {
    console_init();
  }
    create_log_file();

  return client_run();
}
