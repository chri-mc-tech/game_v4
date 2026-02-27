#include "client_core.h"
#include "client_network.h"

#include <iostream>

#include "client_global.h"
#include "shared_crypto.h"

using std::cout;
using std::endl;

int main() {
  if (!initialize_libraries()) {
    return 3;
  }

  if (!config_exist()) {
    cout << "no config file found" << endl;
    create_config_file();
    if (!config_exist()) {
      return 4;
    }
  }

  if (!load_config()) {
    return 5;
  }
  cout << config::uuid << endl;
  cout << config::name << endl;

  if (create_enet_host() != 0) {
    return 6;
  }

  // log_debug(to_string(shared::network::get_packet_type("[1]ciao")));

  client_run();
}
