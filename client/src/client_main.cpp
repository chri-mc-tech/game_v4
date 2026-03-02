#include "client_core.h"
#include "client_network.h"

#include <iostream>

#include "client_config.h"
#include "client_global.h"
#include "shared_crypto.h"

using std::cout;
using std::endl;

int main() {
  if (!initialize_libraries()) {
    return 3;
  }

  if (!config::check()) {
    cout << "no config file found" << endl;
    config::create_config_file();
    if (!config::check()) {
      return 4;
    }
  }

  if (!config::load_config()) {
    return 5;
  }
  cout << global::config::uuid << endl;
  cout << global::config::name << endl;

  if (create_enet_host() != 0) {
    return 6;
  }

  // log_debug(to_string(shared::network::get_packet_type("[1]ciao")));

  client_run();
}
