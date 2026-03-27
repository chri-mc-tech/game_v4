#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <enet/enet.h>
#include <fstream>
#include <thread>
#include <yaml-cpp/yaml.h>

#include "client_logger.h"
#include "client_ui.h"
#include "shared_crypto.h"
#include "shared_utils.h"

bool initialize_libraries() {
  if (enet_initialize() != 0) {
    return false;
  }

  return true;
}

int client_run() {
  global::running = true;

  start_graphics();

  // ui::create_objects_to_render();
  // ui::create_buttons();

  global::main_player.name = global::config::name;
  global::main_player.uuid = global::config::uuid;
  global::main_player.location_x = 0;
  global::main_player.location_y = 0;

  // std::jthread thread_count_frames(count_frames);

  constexpr double TICK_RATE = 40.0;
  constexpr double TICK_TIME = 1.0 / TICK_RATE;

  double accumulator = 0.0;

  auto last = std::chrono::high_resolution_clock::now();

  create_test_cubes();

  while (global::running) {
    if (WindowShouldClose()) {
      global::running = false;
      break;
    }

    auto now = std::chrono::high_resolution_clock::now();
    double delta = std::chrono::duration<double>(now - last).count();
    last = now;

    accumulator += delta;

    enet_loop();
    render();

    while (accumulator >= TICK_TIME)
    {
      // {TICK_RATE} TPS functions

      send_location();
      accumulator -= TICK_TIME;
    }

    if (global::input_string.ends_with("\n")) {global::input_string.clear();}
  }

  CloseWindow();
  return 0;
}

int render() {

  using global::speed;
  using namespace global;


  if (IsWindowResized()) {
    graphics::window_width = GetScreenWidth();
    graphics::window_height = GetScreenHeight();
  }


  if (IsKeyPressed(KEY_ESCAPE)) {
    if (IsCursorHidden()) {
      EnableCursor();
    }
    else {
      DisableCursor();
    }
  }

  if (IsCursorHidden()) {
    if (GetMouseWheelMove() > 0) {
      speed += (speed / 4);
    }

    if (GetMouseWheelMove() < 0) {
      speed -= (speed / 4);

    }


    UpdateCameraPro(&graphics::camera,
            (Vector3){
                IsKeyDown(KEY_W)*speed - IsKeyDown(KEY_S)*speed,
                IsKeyDown(KEY_D)*speed - IsKeyDown(KEY_A)*speed,
                0
            },
            (Vector3){
                GetMouseDelta().x*0.05f,
                GetMouseDelta().y*0.05f,
                0.0f
            }, 0);
  }

  BeginDrawing();
  ClearBackground(SKYBLUE);

  BeginMode3D(graphics::camera);

  render_test_cubes();

  DrawGrid(100, 1.0f);
  EndMode3D();

  DrawFPS(10, 10);
  DrawText(std::to_string(graphics::camera.position.x).c_str(), 10, 40, 30, BLACK);
  DrawText(std::to_string(graphics::camera.position.y).c_str(), 10, 70, 30, BLACK);
  DrawText(std::to_string(graphics::camera.position.z).c_str(), 10, 100, 30, BLACK);

  EndDrawing();

  return 0;
}

void create_test_cubes() {
  for (int x = 0; x < 10; x++) {
    for (int z = 0; z < 10; z++) {
      global::cube_colors[x][z] = (Color){
        static_cast<unsigned char>(rand() % 256),
        static_cast<unsigned char>(rand() % 256),
        static_cast<unsigned char>(rand() % 256),
        255
      };
    }
  }
}

void render_test_cubes() {
  for (int x = 0; x < 10; x++) {
    for (int z = 0; z < 10; z++) {

      DrawCube(
        (Vector3){ static_cast<float>(x) + 0.5f, 0.5f, static_cast<float>(z) + 0.5f },
        1.0f, 1.0f, 1.0f,
        global::cube_colors[x][z]
      );

    }
  }
}

void start_graphics() {
  using namespace global::graphics;

  InitWindow(window_width, window_height, "game");

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetWindowMinSize(640, 360);
  SetWindowMaxSize(7680, 4320);

  camera.position = (Vector3){ 0.0f, 2.0f, 6.0f };
  camera.target = (Vector3){ 0.0f, 1.8f, 0.0f };
  camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
  camera.fovy = 70.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetExitKey(KEY_NULL);

  DisableCursor();

}
