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
    auto frame_start = std::chrono::high_resolution_clock::now();
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

    auto frame_end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(frame_end - frame_start).count();

    if (elapsed < TICK_TIME) {
      std::this_thread::sleep_for(
          std::chrono::duration<double>(TICK_TIME - elapsed)
      );
    }
  }

  return 0;
}

