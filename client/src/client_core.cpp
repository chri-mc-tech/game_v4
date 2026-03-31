#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <enet/enet.h>
#include <fstream>
#include <thread>
#include <yaml-cpp/yaml.h>

#include <raymath.h>

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

  global::main_player.name = global::config::name;
  global::main_player.uuid = global::config::uuid;

  constexpr double TICK_RATE = 40.0;
  constexpr double TICK_TIME = 1.0 / TICK_RATE;

  double accumulator = 0.0;

  auto last = std::chrono::high_resolution_clock::now();

  create_test_cubes();

  global::graphics::font = LoadFontEx("Archivo-SemiBold.ttf", 64, nullptr, 0);

  ui::create_all_buttons();

  while (global::running) {
    if (WindowShouldClose()) {
      global::running = false;
      break;
    }

    auto now = std::chrono::high_resolution_clock::now();
    global::delta_time = std::chrono::duration<double>(now - last).count();
    last = now;

    accumulator += global::delta_time;

    enet_loop();
    update_window();
    update_camera();
    render();

    while (accumulator >= TICK_TIME) {
      send_location();
      accumulator -= TICK_TIME;
    }

    if (global::input_string.ends_with("\n")) { global::input_string.clear(); }
  }

  CloseWindow();
  return 0;
}


int update_window() {
  using global::speed;
  using namespace global;

  if (IsWindowResized()) {
    graphics::window_width = GetScreenWidth();
    graphics::window_height = GetScreenHeight();
  }

  if (global::status_game == STATUS_GAME_PLAYING) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      if (IsCursorHidden()) {
        EnableCursor();
      }
      else {
        DisableCursor();
      }
    }


    if (IsCursorHidden()) {
      float wheel = GetMouseWheelMove();
      if (wheel != 0) {
        speed += (wheel * speed * 30.0f) * static_cast<float>(delta_time);
      }
    }
  }
  return 0;
}

int update_camera() {
  using namespace global;

  if (global::status_game == STATUS_GAME_PLAYING) {
    if (IsCursorHidden()) {
      float frame_speed = speed * static_cast<float>(delta_time);

      if (!check_collision()) {
        next_movement = {
          (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S))) * frame_speed,
          (static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) * frame_speed,
            (static_cast<float>(IsKeyDown(KEY_SPACE)) - static_cast<float>(IsKeyDown(KEY_LEFT_SHIFT))) * frame_speed
        };
      }

      else {
        next_movement = Vector3Subtract({0, 0, 0}, last_movement);
      }

      UpdateCameraPro(&graphics::camera, next_movement,
          (Vector3){GetMouseDelta().x * 0.05f, GetMouseDelta().y * 0.05f, 0.0f},
          0);

      main_player.location = graphics::camera.position;
      main_player.location.y -= camera_height;

      last_movement = next_movement;

    }
  }

  return 0;
}

int render() {
  using global::speed;
  using namespace global;
  using graphics::camera;

  BeginDrawing();

  ClearBackground(BLACK);
  // ClearBackground({ 30, 31, 108, 255 }); // day: SKYBLUE night: 30, 31, 108
  rendering_3D();
  rendering_menu();

  EndDrawing();

  return 0;
}

void rendering_3D() {
  using namespace global;
  using graphics::camera;
  if (global::status_game == STATUS_GAME_PLAYING) {
    BeginMode3D(camera);

    render_test_cubes();

    hitbox = {
      Vector3Add(Vector3(main_player.location), Vector3({- (player_width / 2), 0, - (player_width / 2)})),
      Vector3Add(Vector3(main_player.location), Vector3({+ (player_width / 2), player_height, + (player_width / 2)})),
    };

    DrawBoundingBox(hitbox, RED);

    DrawGrid(100, 1.0f);
    EndMode3D();
  }

}

void rendering_menu() {
  using namespace global;
  using namespace global::graphics;

  DrawFPS(10, 10);
  /*
  DrawText((std::to_string(main_player.location.x).substr(0, 4) + ", " + std::to_string(main_player.location.y).substr(0, 4) + ", " + std::to_string(main_player.location.z).substr(0, 4)).c_str(), 10, 70, 30, BLACK);
  if (check_collision()) {
    DrawText("COLLISION", 10, 120, 30, RED);
  }
  */

  DrawText(std::to_string(global::status_menu).c_str(), 10, 30, 20, GREEN);

  //------------------------------
  // BUTTON TEST
  //------------------------------

  /*
  Rectangle singleplayer_button = {
    (static_cast<float>(window_width) / 2) - 150, (static_cast<float>(window_height) / 2) - 20,
    300, 40
  };

  Rectangle multiplayer_button = {
    (static_cast<float>(window_width) / 2) - 150, (static_cast<float>(window_height) / 2) + 40,
    300, 40
  };

  const char* text = "Singleplayer";

  Vector2 text_size = MeasureTextEx(font, text, 32, 0.5);
*/
  switch (global::status_menu) {
    case STATUS_MENU_MAIN_MENU: {


      ui::button_singleplayer.render();
      ui::button_multiplayer.render();

      if (CheckCollisionPointRec(GetMousePosition(), ui::button_singleplayer.rect)) {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
          std::cout << "test\n";
        }
      }
    }
  }
}

void create_test_cubes() {
  for (int x = 0; x < 50; x++) {
    for (int z = 0; z < 50; z++) {
      global::cube_colors[x][z] = (Color){
        static_cast<unsigned char>(rand() % 10),
        static_cast<unsigned char>(rand() % 30),
        static_cast<unsigned char>(rand() % 200),
        255
      };
      Block b;
      b.x = x;
      b.y = 0;
      b.z = z;
      b.solid = true;
      global::world.push_back(b);
    }
  }
}

void render_test_cubes() {
  for (int x = 0; x < 50; x++) {
    for (int z = 0; z < 50; z++) {

      DrawCube(
        (Vector3){ static_cast<float>(x) + 0.5f, 0, static_cast<float>(z) + 0.5f},
        1.0f, 1.0f, 1.0f,
        global::cube_colors[x][z]
      );

    }
  }
}

void start_graphics() {
  using namespace global::graphics;
  using namespace global;

  InitWindow(window_width, window_height, "game");

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetWindowMinSize(640, 360);
  SetWindowMaxSize(7680, 4320);

  camera.position = (Vector3){ 0.0f, camera_height + 0.6f, 0.0f };
  camera.target = (Vector3){ 0.0f, camera_height + 0.6f, 1.0f };
  camera.up = (Vector3){ 0.0f, 2.0f, 0.0f };
  camera.fovy = 70.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetExitKey(KEY_NULL);

  EnableCursor();


}

void draw_player_model() {
  using namespace global;
  DrawCube(
  Vector3Add(Vector3(main_player.location), Vector3({0, player_height / 2, 0})),
  player_width, player_height, player_width, {0, 0, 0, 80});

}

bool check_collision() {
  using namespace global;

  for (auto &b : world) {
    if (!b.solid) continue;

    if (abs(b.x - main_player.location.x) > 2) continue;
    if (abs(b.y - main_player.location.y) > 3) continue;
    if (abs(b.z - main_player.location.z) > 2) continue;

    BoundingBox blockBox = {
      (Vector3){ float(b.x), float(b.y) - 0.5f, float(b.z) },
      (Vector3){ float(b.x) + 1.0f, float(b.y) + 0.5f, float(b.z) + 1.0f }
    };

    DrawBoundingBox(blockBox, RED);

    if (CheckCollisionBoxes(hitbox, blockBox)) {
      return true;
    }
  }
  return false;

}