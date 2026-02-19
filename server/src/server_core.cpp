#include "server_core.h"
#include "server_network.h"

#include <enet/enet.h>

#include "server_global.h"

bool initialize_libraries() {
  // if !=0: error
  if (enet_initialize() != 0) {
    return false;
  }

  return true;
}

int server_run() {
  global::running = true;

  while (global::running) {
    enet_loop();

  }

  return 0;
}
