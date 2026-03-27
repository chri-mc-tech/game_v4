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
      update_location();
      send_location();
      accumulator -= TICK_TIME;
    }

    // global::frames ++;

    if (global::input_string.ends_with("\n")) {global::input_string.clear();}
  }

  return 0;
}

int render() {

  using global::speed;

  if (IsWindowResized()) {
    global::graphics::window_width = GetScreenWidth();
    global::graphics::window_height = GetScreenHeight();
  }

  static Color cube_colors[10][10];
  static bool generated = false;

  if (!generated) {
    for (int x = 0; x < 10; x++) {
      for (int z = 0; z < 10; z++) {
        cube_colors[x][z] = (Color){
          (unsigned char)(rand() % 256),
          (unsigned char)(rand() % 256),
          (unsigned char)(rand() % 256),
          255
        };
      }
    }
    generated = true;
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
    UpdateCameraPro(&global::graphics::camera,
            (Vector3){
                IsKeyDown(KEY_W)*speed - IsKeyDown(KEY_S)*speed,
                IsKeyDown(KEY_D)*speed - IsKeyDown(KEY_A)*speed,
                IsKeyDown(KEY_SPACE)*speed - IsKeyDown(KEY_LEFT_SHIFT)*speed,
            },
            (Vector3){
                GetMouseDelta().x*0.05f,
                GetMouseDelta().y*0.05f,
                0.0f
            }, 0);


    BeginDrawing();
    ClearBackground(SKYBLUE);

    BeginMode3D(global::graphics::camera);

    for (int x = 0; x < 10; x++) {
      for (int z = 0; z < 10; z++) {

        DrawCube(
          (Vector3){ (float)x + 0.5f, 0.5f, (float)z + 0.5f },
          1.0f, 1.0f, 1.0f,
          cube_colors[x][z]
        );

      }
    }
  }

  DrawGrid(50, 1.0f);
  EndMode3D();

  DrawFPS(10, 10);

  EndDrawing();

  return 0;
}

void update_location() {
  if (global::status_connection == STATUS_CONNECTION_ENCRYPTED) {
    if (global::status_game == STATUS_GAME_PLAYING) {
      if (!global::chat_open) {

        if (IsKeyDown(KEY_W)) {
          global::main_player.location_y -= 4;
        }

        if (IsKeyDown(KEY_S)) {
          global::main_player.location_y += 4;
        }

        if (IsKeyDown(KEY_A)) {
          global::main_player.location_x -= 4;
        }

        if (IsKeyDown(KEY_D)) {
          global::main_player.location_x += 4;
        }
      }
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

}
