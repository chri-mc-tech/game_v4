#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <enet/enet.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <yaml-cpp/yaml.h>

#include "client_config.h"
#include "client_logger.h"
#include "client_textures.h"
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
  textures::load_textures();


  while (global::running) {
    if (WindowShouldClose()) {
      global::running = false;
      break;
    }

    auto now = std::chrono::high_resolution_clock::now();
    global::delta_time = std::chrono::duration<float>(now - last).count();
    last = now;

    if (global::delta_time > 0.1) global::delta_time = 0.1;

    accumulator += global::delta_time;

    if (accumulator > 1.0) {
      accumulator = TICK_TIME;
    }

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

void update_input() {
  static bool f3_used = false;

  if (IsKeyPressed(KEY_F3)) {
    f3_used = false;
  }

  if (IsKeyDown(KEY_F3)) {
    if (IsKeyPressed(KEY_G)) {
      switch (global::debug_grid) {
        case DEBUG_GRID_OFF: global::debug_grid = DEBUG_GRID_BLOCKS; break;
        case DEBUG_GRID_BLOCKS: global::debug_grid = DEBUG_GRID_CHUNKS; break;
        case DEBUG_GRID_CHUNKS: global::debug_grid = DEBUG_GRID_OFF; break;
        default: break;
      }
      f3_used = true;
    }
    else if (IsKeyPressed(KEY_H)) {
      global::show_hitbox = !global::show_hitbox;
      f3_used = true;
    }
    else if (IsKeyPressed(KEY_C)) {
      global::show_collision_hitbox = !global::show_collision_hitbox;
      f3_used = true;
    }
    else if (IsKeyPressed(KEY_F)) {
      using global::texture_filter;

      switch (texture_filter) {
        case TEXTURE_FILTER_POINT: texture_filter = TEXTURE_FILTER_BILINEAR; break;
        case TEXTURE_FILTER_BILINEAR: texture_filter = TEXTURE_FILTER_TRILINEAR; break;
        case TEXTURE_FILTER_TRILINEAR: texture_filter = TEXTURE_FILTER_POINT; break;
        default: break;
      }
      textures::update_filter();
      f3_used = true;
    }
  }
  else if (IsKeyReleased(KEY_F3) && !f3_used) {
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
}

void update_window() {
  using namespace global;

  if (IsWindowResized()) {
    graphics::window_width = GetScreenWidth();
    graphics::window_height = GetScreenHeight();
  }

  if (global::status_game == STATUS_GAME_PLAYING) {
    if (IsKeyPressed(KEY_ESCAPE)) {
      if (global::status_menu == STATUS_MENU_IN_GAME) {
        global::status_menu = STATUS_MENU_PAUSE;
        EnableCursor();
      }
      else {
        global::status_menu = STATUS_MENU_IN_GAME;
        DisableCursor();
      }
    }
  }
}


void update_camera() {
  using namespace global;

  if (global::status_game == STATUS_GAME_PLAYING) {
    if (IsKeyDown(KEY_LEFT_CONTROL)) {
      if (is_grounded) {
        now_max_horizontal_speed = max_run_horizontal_speed;
      }
    }
    else {
      now_max_horizontal_speed = max_walk_horizontal_speed;
    }

    // delta time ≈ 0.00xxx

    Vector3 old_player_location = main_player.location;
    Vector3 old_camera_location = graphics::camera.position;
    Vector3 old_camera_target = graphics::camera.target;

    if (!is_grounded) {
      velocity_Y -= GRAVITY * delta_time;
    } else {
      velocity_Y = -GRAVITY * delta_time;
    }


    if (IsKeyDown(KEY_SPACE) && is_grounded) {
      velocity_Y = jump_speed;
      is_grounded = false;
    }

    if (velocity_Y < -100) {
      velocity_Y = -100;
    }

    calculate_player_speed();

    Vector3 movement = {0, 0, 0};
    if (IsCursorHidden()) {
      movement = {
        current_velocity_forward * delta_time,
        current_velocity_side * delta_time,
        0
        };
    }

    movement.z = velocity_Y * delta_time;

    if (main_player.location.y < -100) {
      teleport_player({0.5, 3, 0.5});
    }

    update_player_location(movement, {0, 0, 0});

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

    bool collision_x = false;
    bool collision_y = false;
    bool collision_z = false;

    for (const auto& chunk : world::chunks) {
      if (abs(chunk.second.x - main_player.chunk_x) > 2) continue;
      if (abs(chunk.second.z - main_player.chunk_z) > 2) continue;

      for (int x = 0; x < SIZE_X; x++) {
        for (int y = 0; y < SIZE_Y; y++) {
          for (int z = 0; z < SIZE_Z; z++) {

            int global_x = chunk.second.x * 16 + x;
            int global_z = chunk.second.z * 16 + z;

            if (abs(global_x - main_player.block_x) > 2) continue;
            if (abs(y - main_player.block_y) > 3) continue;
            if (abs(global_z - main_player.block_z) > 2) continue;

            int block_type = chunk.second.blocks[x][y][z];

            if (block_type == 0) continue;

            BoundingBox block_hitbox = {
              {static_cast<float>(global_x), static_cast<float>(y), static_cast<float>(global_z)},
              {static_cast<float>(global_x) + 1.0f, static_cast<float>(y) + 1.0f, static_cast<float>(global_z) + 1.0f}
            };

            if (CheckCollisionBoxes(last_collision_hitbox_x, block_hitbox)) {
              collision_x = true;
            }
            if (CheckCollisionBoxes(last_collision_hitbox_y, block_hitbox)) {
              collision_y = true;
            }
            if (CheckCollisionBoxes(last_collision_hitbox_z, block_hitbox)) {
              collision_z = true;
            }
          }
        }
      }
    }

    if (collision_x) {
      graphics::camera.position.x = old_camera_location.x;
      graphics::camera.target.x = old_camera_target.x;
    }
    if (collision_y) {
      graphics::camera.position.y = old_camera_location.y;
      graphics::camera.target.y = old_camera_target.y;

      if (velocity_Y < 0) {
        is_grounded = true;
      }
    } else {
      is_grounded = false;
    }

    if (collision_z) {
      graphics::camera.position.z = old_camera_location.z;
      graphics::camera.target.z = old_camera_target.z;
    }

    Vector3 rotation = {0, 0, 0};

    if (IsCursorHidden()) {
      rotation = {GetMouseDelta().x * 0.05f, GetMouseDelta().y * 0.05f, 0.0f};
    }

    update_player_location({0, 0, 0}, rotation);

    main_player.hitbox = {
      Vector3Add(Vector3(main_player.location), Vector3({- (player_width / 2), 0, - (player_width / 2)})),
      Vector3Add(Vector3(main_player.location), Vector3({+ (player_width / 2), player_height, + (player_width / 2)})),
    };
  }
}


int render() {
  using namespace global;
  using graphics::camera;

  BeginDrawing();

  if (global::status_game == STATUS_GAME_PLAYING) {
    ClearBackground({30, 31, 108});
  }
  else {
    ClearBackground(BLACK);
  }

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

    world::render_chunk("0 0");
    world::render_chunk("-1 -1");

    if (show_hitbox) {
      DrawBoundingBox(main_player.hitbox, RED);
    }

    if (show_collision_hitbox) {
      DrawBoundingBox(last_collision_hitbox_x, BLUE);
      DrawBoundingBox(last_collision_hitbox_y, GREEN);
      DrawBoundingBox(last_collision_hitbox_z, YELLOW);
    }

    switch (global::debug_grid) {
      case DEBUG_GRID_BLOCKS: DrawGrid(800, 1.0f); break;
      case DEBUG_GRID_CHUNKS: DrawGrid(50, 16.0f); break;
      default: break;
    }

    for (const auto& loop_player: online_players) {
      DrawBoundingBox(loop_player.second.hitbox, RED);

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

    float x = main_player.location.x;
    float y = main_player.location.y;
    float z = main_player.location.z;

    int block_x = static_cast<int>(floor(main_player.location.x));
    int block_y = static_cast<int>(floor(main_player.location.y));
    int block_z = static_cast<int>(floor(main_player.location.z));
    debug_menu_advanced_string = "coords: " + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z);

    string is_grounded_string;
    if (is_grounded) {is_grounded_string = "true";}
    else {is_grounded_string = "false";}
    debug_menu_advanced_string += "\nis grounded?: " + is_grounded_string;

    debug_menu_advanced_string += "\ntexture filter: ";
    switch (texture_filter) {
      case TEXTURE_FILTER_POINT: debug_menu_advanced_string += "POINT"; break;
      case TEXTURE_FILTER_BILINEAR: debug_menu_advanced_string += "BILINEAR"; break;
      case TEXTURE_FILTER_TRILINEAR: debug_menu_advanced_string += "TRILINEAR"; break;
      default: break;
    }

    debug_menu_string += "\nblock: " + std::to_string(block_x) + " " + std::to_string(block_y) + " " + std::to_string(block_z);

    DrawTextEx(font, debug_menu_string.c_str(), {10, 10}, 24, 0.5, WHITE);

    if (global::debug_menu == DEBUG_MENU_ADVANCED) {
      DrawTextEx(font, debug_menu_advanced_string.c_str(), {static_cast<float>(window_width)/2, 10}, 24, 0.5, WHITE);
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
        input_string.clear();
      }

      break;
    }
    case STATUS_MENU_DISCONNECTED_FROM_SERVER: {
      ui::draw_centered_text("Disconnected from server", window_width/2, window_height/2 - 50, WHITE);
      ui::button_continue.render(window_width / 2 - 125, window_height - 70);

      if (is_button_clicked(ui::button_continue) || IsKeyPressed(KEY_ENTER)) {
        global::status_menu = STATUS_MENU_MAIN_MENU;
      }
      break;
    }
    case STATUS_MENU_PAUSE: {
      ui::button_settings.render(
        window_width / 2 - static_cast<int>(ui::button_settings.rect.width / 2),
        window_height / 2 - static_cast<int>(ui::button_settings.rect.height / 2)
        );
      ui::button_quit.render(
        window_width / 2 - static_cast<int>(ui::button_quit.rect.width / 2),
        window_height / 2 - static_cast<int>(ui::button_quit.rect.height / 2) + 60
        );

      if (is_button_clicked(ui::button_settings)) {
        global::status_menu = STATUS_MENU_SETTINGS;
      }
      if (is_button_clicked(ui::button_quit)) {
        enet_peer_disconnect_later(enet::connected_server_peer, 0);
      }
      break;
    }
    case STATUS_MENU_SETTINGS: {
      ui::button_back.render(
        window_width / 2 - static_cast<int>(ui::button_back.rect.width / 2),
        window_height - 70
        );

      if (is_button_clicked(ui::button_back)) {
        global::status_menu = STATUS_MENU_PAUSE;
      }

      break;
    }
    default: break;;
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

  camera.position = (Vector3){ 0.0f, 1 + camera_height + 0.2f, 0.0f }; // + 0.2f is to not spawn inside the block
  camera.target = (Vector3){ 0.0f, 1 + camera_height + 0.2f, 1.0f };
  camera.up = (Vector3){ 0.0f, 3.0f, 0.0f };
  camera.fovy = 90.0f;
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
    input_string += static_cast<char>(char_pressed);
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

void update_player_location(const Vector3 t_movement, const Vector3 t_rotation) {
  using namespace global;

  UpdateCameraPro(&graphics::camera, t_movement, t_rotation, 0);

  main_player.location = graphics::camera.position;
  main_player.location.y -= camera_height;

  main_player.block_x = static_cast<int>(floor(main_player.location.x));
  main_player.block_y = static_cast<int>(floor(main_player.location.y));
  main_player.block_z = static_cast<int>(floor(main_player.location.z));

  main_player.chunk_x = static_cast<int>(floor(static_cast<double>(main_player.block_x) / 16.0));
  main_player.chunk_z = static_cast<int>(floor(static_cast<double>(main_player.block_z) / 16.0));
}

void teleport_player(Vector3 new_pos) {
  using namespace global;
  using namespace global::graphics;

  Vector3 forward = Vector3Subtract(camera.target, camera.position);
  camera.position = new_pos;
  camera.target = Vector3Add(new_pos, forward);

  main_player.location = camera.position;
  main_player.location.y -= camera_height;

  main_player.block_x = static_cast<int>(floor(main_player.location.x));
  main_player.block_y = static_cast<int>(floor(main_player.location.y));
  main_player.block_z = static_cast<int>(floor(main_player.location.z));

  main_player.chunk_x = static_cast<int>(floor(static_cast<double>(main_player.block_x) / 16.0));
  main_player.chunk_z = static_cast<int>(floor(static_cast<double>(main_player.block_z) / 16.0));

  velocity_Y = 0.0f;
  is_grounded = false;

  main_player.hitbox = {
    Vector3Add(main_player.location, {- (player_width / 2), 0, - (player_width / 2)}),
    Vector3Add(main_player.location, {+ (player_width / 2), player_height, + (player_width / 2)}),
  };
}

void calculate_player_speed() {
  using namespace global;

  float w_s_movement = static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S));
  float d_a_movement = static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A));

  if (w_s_movement != 0) {
    current_velocity_forward += w_s_movement * horizontal_acceleration * delta_time;
  }
  else {
    if (current_velocity_forward > 0) {
      if (current_velocity_forward - horizontal_friction * delta_time < 0) {
        current_velocity_forward = 0;
      }
      else {
        current_velocity_forward -= horizontal_friction * delta_time;
      }
    }
    else if (current_velocity_forward < 0) {
      if (current_velocity_forward + horizontal_friction * delta_time > 0) {
        current_velocity_forward = 0;
      }
      else {
        current_velocity_forward += horizontal_friction * delta_time;
      }
    }
    else {
      current_velocity_forward = 0;
    }
  }

  if (d_a_movement != 0) {
    current_velocity_side += d_a_movement * horizontal_acceleration * delta_time;
  }
  else {
    if (current_velocity_side > 0) {
      if (current_velocity_side - horizontal_friction * delta_time < 0) {
        current_velocity_side = 0;
      }
      else {
        current_velocity_side -= horizontal_friction * delta_time;
      }
    }
    else if (current_velocity_side < 0) {
      if (current_velocity_side + horizontal_friction * delta_time > 0) {
        current_velocity_side = 0;
      }
      else {
        current_velocity_side += horizontal_friction * delta_time;
      }
    }
    else {
      current_velocity_side = 0;
    }
  }

  current_velocity_forward = Clamp(current_velocity_forward, -now_max_horizontal_speed, now_max_horizontal_speed);
  current_velocity_side = Clamp(current_velocity_side, -now_max_horizontal_speed, now_max_horizontal_speed);

}
