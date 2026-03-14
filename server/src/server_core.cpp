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

  const int TICK_RATE = 40;
  const int TICK_TIME_MS = 1000 / TICK_RATE;

  while (global::running)
  {
    auto start = std::chrono::steady_clock::now();

    // {TICK_RATE} TPS functions
    enet_loop();
    send_players_location();

    auto end = std::chrono::steady_clock::now();

    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (elapsed < TICK_TIME_MS)
    {
      std::this_thread::sleep_for(
          std::chrono::milliseconds(TICK_TIME_MS - elapsed));
    }
  }

  return 0;
}