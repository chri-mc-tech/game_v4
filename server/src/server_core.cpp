#include "server_core.h"

#include <thread>

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

int server_run()
{
  global::running = true;

  constexpr double TICK_RATE = 40.0;
  constexpr double TICK_TIME = 1.0 / TICK_RATE;

  double accumulator = 0.0;

  auto last = std::chrono::high_resolution_clock::now();

  while (global::running) {
    auto now = std::chrono::high_resolution_clock::now();
    double delta = std::chrono::duration<double>(now - last).count();
    last = now;

    accumulator += delta;

    while (accumulator >= TICK_TIME) {
      // {TICK_RATE} TPS functions
      enet_loop();
      send_players_location();
      accumulator -= TICK_TIME;
    }
  }

  return 0;
}