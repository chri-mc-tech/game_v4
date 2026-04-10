#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <enet/enet.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <yaml-cpp/yaml.h>

#include <raymath.h>

#include "client_config.h"
#include "client_logger.h"
#include "client_ui.h"
#include "client_world.h"
#include "shared_crypto.h"
#include "shared_network.h"
#include "shared_utils.h"

bool initialize_libraries() {
  if (enet_initialize() != 0) {
    return false;
  }

  return true;
}

int client_run() {
  global::running = true;

  // global::background = LoadTexture("background.png");

  start_graphics();

  global::main_player.name = config::name;
  global::main_player.uuid = config::uuid;

  constexpr double TICK_RATE = 40.0;
  constexpr double TICK_TIME = 1.0 / TICK_RATE;
  double accumulator = 0.0;
  auto last = std::chrono::high_resolution_clock::now();

  // create_test_cubes();

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
    update_input();
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

int update_input() {
  if (IsKeyPressed(KEY_F8)) {
    switch (global::debug_grid) {
      case DEBUG_GRID_OFF: global::debug_grid = DEBUG_GRID_BLOCKS; break;
      case DEBUG_GRID_BLOCKS:global::debug_grid = DEBUG_GRID_CHUNKS; break;
      case DEBUG_GRID_CHUNKS:global::debug_grid = DEBUG_GRID_OFF; break;

    }
  }

  if (IsKeyPressed(KEY_F3)) {
    if (global::debug_menu == DEBUG_MENU_CLOSED) {
      if (IsKeyDown(KEY_LEFT_SHIFT)) {
        global::debug_menu = DEBUG_MENU_ADVANCED;
      }
      else {
        global::debug_menu = DEBUG_MENU_DEFAULT;
      }
    }
    else {
      global::debug_menu = DEBUG_MENU_CLOSED;
    }
  }

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
        speed += (wheel * speed * 50.0f) * static_cast<float>(delta_time);
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

      Vector3 movement = {
        (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S))) * frame_speed,
        (static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) * frame_speed,
        (static_cast<float>(IsKeyDown(KEY_SPACE)) - static_cast<float>(IsKeyDown(KEY_LEFT_SHIFT))) * frame_speed
        };

      Vector3 old_player_location = main_player.location;
      Vector3 old_camera_location = graphics::camera.position;
      Vector3 old_camera_target = graphics::camera.target;

      UpdateCameraPro(&graphics::camera, movement,
        (Vector3){0, 0, 0},
        0);

      main_player.location = graphics::camera.position;
      main_player.location.y -= camera_height;
      main_player.block_x = static_cast<int>(floor(main_player.location.x));
      main_player.block_y = static_cast<int>(floor(main_player.location.y));
      main_player.block_z = static_cast<int>(floor(main_player.location.z));

      main_player.chunk_x = static_cast<int>(floor(static_cast<double>(main_player.block_x) / 16.0));
      main_player.chunk_z = static_cast<int>(floor(static_cast<double>(main_player.block_z) / 16.0));

      last_collision_hitbox_x = {
        Vector3Add(main_player.hitbox.min, {(main_player.location.x - old_player_location.x), 0, 0}),
        Vector3Add(main_player.hitbox.max, {(main_player.location.x - old_player_location.x), 0, 0})
      };
      last_collision_hitbox_y = {
        Vector3Add(main_player.hitbox.min, {0, (main_player.location.y - old_player_location.y), 0}),
        Vector3Add(main_player.hitbox.max, {0, (main_player.location.y - old_player_location.y), 0})
      };
      last_collision_hitbox_z = {
        Vector3Add(main_player.hitbox.min, {0, 0, (main_player.location.z - old_player_location.z)}),
        Vector3Add(main_player.hitbox.max, {0, 0, (main_player.location.z - old_player_location.z)})
      };

      bool colliosion_x = false;
      bool colliosion_y = false;
      bool colliosion_z = false;

      for (const auto& chunk : world::chunks) {
        if (abs(chunk.second.x - main_player.chunk_x) > 2) continue;
        if (abs(chunk.second.z - main_player.chunk_z) > 2) continue;

        for (int x = 0; x < SIZE_X; x++) {
          for (int y = 0; y < SIZE_Y; y++) {
            for (int z = 0; z < SIZE_Z; z++) {

              float global_x = chunk.second.x * 16 + x;
              float global_z = chunk.second.z * 16 + z;

              if (abs(global_x - main_player.block_x) > 2) continue;
              if (abs(y - main_player.block_y) > 3) continue;
              if (abs(global_z - main_player.block_z) > 2) continue;

              int block_type = chunk.second.blocks[x][y][z];

              if (block_type == 0) continue;

              BoundingBox block_hitbox = {
                (Vector3){ global_x, y - 0.5f, global_z },
                (Vector3){ global_x + 1.0f, y + 0.5f, global_z + 1.0f }
              };

              if (CheckCollisionBoxes(last_collision_hitbox_x, block_hitbox)) {
                colliosion_x = true;
              }
              if (CheckCollisionBoxes(last_collision_hitbox_y, block_hitbox)) {
                colliosion_y = true;
              }
              if (CheckCollisionBoxes(last_collision_hitbox_z, block_hitbox)) {
                colliosion_z = true;
              }

            }
          }
        }
      }

      if (colliosion_x) {
        graphics::camera.position.x = old_camera_location.x;
        graphics::camera.target.x = old_camera_target.x;
      }
      if (colliosion_y) {
        graphics::camera.position.y = old_camera_location.y;
        graphics::camera.target.y = old_camera_target.y;

        // using Z as vertical axis because movement is camera-relative, not global axes:
        // X = forward/backward, Y = left/right, Z = up/down
        if (movement.z < 0) {
          is_grounded = true;
        }

      }
      else {is_grounded = false;}

      if (colliosion_z) {
        graphics::camera.position.z = old_camera_location.z;
        graphics::camera.target.z = old_camera_target.z;
      }

      UpdateCameraPro(&graphics::camera, {0, 0, 0},
      (Vector3){GetMouseDelta().x * 0.05f, GetMouseDelta().y * 0.05f, 0.0f},
      0);

      main_player.location = graphics::camera.position;
      main_player.location.y -= camera_height;
      main_player.block_x = static_cast<int>(floor(main_player.location.x));
      main_player.block_y = static_cast<int>(floor(main_player.location.y));
      main_player.block_z = static_cast<int>(floor(main_player.location.z));

      main_player.chunk_x = static_cast<int>(floor(static_cast<double>(main_player.block_x) / 16.0));
      main_player.chunk_z = static_cast<int>(floor(static_cast<double>(main_player.block_z) / 16.0));

      main_player.hitbox = {
        Vector3Add(Vector3(main_player.location), Vector3({- (player_width / 2), 0, - (player_width / 2)})),
        Vector3Add(Vector3(main_player.location), Vector3({+ (player_width / 2), player_height, + (player_width / 2)})),
      };
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

  // DrawTexture(background, 0, 0, WHITE);

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

    // render_test_cubes();
    world::render_chunk("0 0");
    world::render_chunk("-1 -1");



    //DrawBoundingBox(main_player.hitbox, RED);

    DrawBoundingBox(last_collision_hitbox_x, BLUE);
    DrawBoundingBox(last_collision_hitbox_y, GREEN);
    DrawBoundingBox(last_collision_hitbox_z, YELLOW);


    switch (global::debug_grid) {
      case DEBUG_GRID_BLOCKS: DrawGrid(800, 1.0f); break;
      case DEBUG_GRID_CHUNKS: DrawGrid(50, 16.0f); break;

    }

    EndMode3D();
  }

}

void rendering_menu() {
  using namespace global;
  using namespace global::graphics;

  if (global::debug_menu >= DEBUG_MENU_DEFAULT) {
    string debug_menu_string;
    string debug_menu_advanced_string;

    debug_menu_string =
      "fps: " + std::to_string(GetFPS()) +
      "\nmenu: " + std::to_string(global::status_menu) +
      "\nconn: " + std::to_string(global::status_connection) +
      "\ngame: " + std::to_string(global::status_game);


    if (global::status_game == STATUS_GAME_PLAYING) {
      float x = main_player.location.x;
      float y = main_player.location.y;
      float z = main_player.location.z;

      int block_x = static_cast<int>(floor(main_player.location.x));
      int block_y = static_cast<int>(floor(main_player.location.y));
      int block_z = static_cast<int>(floor(main_player.location.z));
      debug_menu_advanced_string += "\ncoords: " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);

      /*
      string is_grounded_string;
      if (is_grounded) {is_grounded_string = "true";}
      else {is_grounded_string = "false";}
      DrawText(("is grounded?: " + is_grounded_string).c_str(),
        10, 160, 20, WHITE);
        */


      debug_menu_string += "\nblock: " + std::to_string(block_x) + " " + std::to_string(block_y) + " " + std::to_string(block_z);
    }

    DrawTextEx(font, debug_menu_string.c_str(), {10, 10}, 24, 0.5, WHITE);
    if (global::debug_menu == DEBUG_MENU_ADVANCED) {

      DrawTextEx(font, debug_menu_advanced_string.c_str(), {static_cast<float>(window_width/2), 10}, 24, 0.5, WHITE);
    }
  }

  switch (global::status_menu) {
    case STATUS_MENU_WAITING_USER_INPUT_NAME: {
      get_keyboard_input();

      ui::draw_centered_text("Username:", window_width/2, window_height/2 - 50, WHITE);
      ui::draw_centered_text(input_string, window_width/2, window_height/2, WHITE);
      ui::button_continue.render(window_width / 2 - 125, window_height - 70);

      if (is_button_clicked(ui::button_continue) || IsKeyPressed(KEY_ENTER)) {
        if (shared::utils::is_valid_nickname(input_string)) {
          config::save_new_nickname(input_string);
          input_string.clear();
          global::status_menu = STATUS_MENU_MAIN_MENU;
        }
      }

      break;
    }
    case STATUS_MENU_MAIN_MENU: {

      ui::button_singleplayer.render(window_width/2 - 100,window_height/2 - 60);
      ui::button_multiplayer.render(window_width/2 - 100, window_height/2);

      if (is_button_clicked(ui::button_multiplayer)) {
        global::status_menu = STATUS_MENU_MULTIPLAYER;
      }

      break;
    }

    case STATUS_MENU_MULTIPLAYER: {

      ui::button_add_server.render(50, window_height - 60);
      ui::button_remove_server.render(350, window_height - 60);
      ui::button_direct_connect.render(window_width - 300, window_height - 60);

      if (is_button_clicked(ui::button_direct_connect)) {
        global::status_menu = STATUS_MENU_DIRECT_CONNECT;
      }
      break;
    }
    case STATUS_MENU_DIRECT_CONNECT: {
      get_keyboard_input();

      ui::draw_centered_text("Server IP:", window_width/2, window_height/2 - 50, WHITE);
      ui::draw_centered_text(input_string, window_width/2, window_height/2, WHITE);
      ui::button_continue.render(window_width / 2 - 125, window_height - 70);

      if (is_button_clicked(ui::button_continue) || IsKeyPressed(KEY_ENTER)) {
        global::status_menu = STATUS_MENU_CONNECTING;

        if (input_string.empty()) {
          connect_to_server("127.0.0.1");
          break;
        }

        if (input_string.find(':') == string::npos) {
          connect_to_server(input_string);
        }
        else {
          auto i = input_string.find(':');
          string ip = input_string.substr(0, i);
          string port = input_string.substr(i + 1);

          connect_to_server(ip, port);
        }
        input_string.clear();
      }

      break;
    }

    case STATUS_MENU_CONNECTING: {
      ui::draw_centered_text("connecting", window_width/2, window_height/2, WHITE);
      if (std::chrono::steady_clock::now() - enet::start_connection_time >= std::chrono::seconds(6)) {
        if (global::status_connection == STATUS_CONNECTION_NOT_CONNECTED) {
          global::status_menu = STATUS_MENU_DISCONNECTED_FROM_SERVER;
        }
      }

      break;
    }

    case STATUS_MENU_WAITING_USER_INPUT_PASSWORD: {
      get_keyboard_input();

      ui::draw_centered_text("Password:", window_width/2, window_height/2 - 50, WHITE);
      ui::draw_centered_text(input_string, window_width/2, window_height/2, WHITE);
      ui::button_continue.render(window_width / 2 - 125, window_height - 70);

      if (is_button_clicked(ui::button_continue) || IsKeyPressed(KEY_ENTER)) {
        global::status_menu = STATUS_MENU_VOID;
        auto hashed_pass = shared::crypto::hash_password(input_string);
        log_info(input_string);
        log_info(hashed_pass);

        shared::network::send_packet(enet::connected_server_peer, PKT_FROM_CLIENT_HASHED_PASSWORD, hashed_pass, 2, ENET_PACKET_FLAG_RELIABLE, &encryption_key);
      }

      break;
    }
    case STATUS_MENU_DISCONNECTED_FROM_SERVER: {
      ui::draw_centered_text("Disconnected from server", window_width/2, window_height/2 - 50, WHITE);
      ui::button_continue.render(window_width / 2 - 125, window_height - 70);

      if (is_button_clicked(ui::button_continue) || IsKeyPressed(KEY_ENTER)) {
        global::status_menu = STATUS_MENU_MAIN_MENU;
      }
    }


  }

}


void start_graphics() {
  using namespace global::graphics;
  using namespace global;

  InitWindow(window_width, window_height, "game");

  auto logo = LoadImage("logo.png");
  SetWindowIcon(logo);

  SetWindowState(FLAG_WINDOW_RESIZABLE);
  SetWindowMinSize(640, 360);
  SetWindowMaxSize(7680, 4320);

  camera.position = (Vector3){ 0.0f, camera_height + 0.6f, 0.0f };
  camera.target = (Vector3){ 0.0f, camera_height + 0.6f, 1.0f };
  camera.up = (Vector3){ 0.0f, 2.0f, 0.0f };
  camera.fovy = 70.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  SetExitKey(KEY_NULL);
}

void get_keyboard_input() {
  using namespace global;

  auto char_pressed = GetCharPressed();
  auto key_pressed = GetKeyPressed();

  if (key_pressed == KEY_BACKSPACE) {
    if (!input_string.empty()) {
      input_string.pop_back();
    }
  }

  if (char_pressed != 0) {
    input_string += char_pressed;
  }
}

bool is_button_clicked(const ui::Button &button) {
  if (CheckCollisionPointRec(GetMousePosition(), button.rect)) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      return true;
    }
  }
  return false;
}