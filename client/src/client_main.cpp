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


  if (!shared::utils::is_valid_uuid(config::uuid)) {
    return 8;
  }

  if (create_enet_host() != 0) {
    return 6;
  }

  if (config::debug_console) {
    console_init();
  }
  create_log_file();

  if (!config::name.empty()) {
    global::status_menu = STATUS_MENU_MAIN_MENU;
  }

  return client_run();
}
