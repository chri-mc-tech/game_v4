#include "server_core.h"
#include "server_logger.h"
#include "shared_network.h"

#include <iostream>

#include "server_global.h"
#include "server_network.h"

using namespace std;

int main() {
  if (!initialize_libraries()) {
    return 1;
  }

  if (create_enet_host() != 0) {
    return 1;
  }

  log_info("info_test");
  log_warn("warn_test");
  log_error("error_test");
  log_debug("debug_test");

  // log_debug(to_string(shared::network::get_packet_type("[1]ciao")));

  server_run();
}
