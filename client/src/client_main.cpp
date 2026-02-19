#include "client_core.h"
#include "client_network.h"

#include <iostream>

using namespace std;

int main() {
  if (!initialize_libraries()) {
    return 1;
  }

  if (create_enet_host() != 0) {
    return 1;
  }

  // log_debug(to_string(shared::network::get_packet_type("[1]ciao")));

  client_run();
}
