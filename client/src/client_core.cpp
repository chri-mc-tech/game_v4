#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <enet/enet.h>
#include <fstream>
#include <thread>
#include <yaml-cpp/yaml.h>

#include "client_logger.h"
#include "client_ui.h"
#include "shared_crypto.h"
#include "shared_utils.h"

bool initialize_libraries() {
  // if !=0: error
  if (enet_initialize() != 0) {
    return false;
  }

  // if false: error
  if (! SDL_Init(SDL_INIT_VIDEO)) {
    return false;
  }

  // if false: error
  if (! TTF_Init()) {
    return false;
  }

  return true;
}

int client_run() {
  global::running = true;

  start_sdl();

  ui::create_objects_to_render();
  ui::create_buttons();

  global::main_player.name = global::config::name;
  global::main_player.uuid = global::config::uuid;
  global::main_player.location_x = 0;
  global::main_player.location_y = 0;

  std::jthread thread_count_frames(count_frames);

  constexpr double TICK_RATE = 40.0;
  constexpr double TICK_TIME = 1.0 / TICK_RATE;

  double accumulator = 0.0;

  auto last = std::chrono::high_resolution_clock::now();

  while (global::running) {
    auto now = std::chrono::high_resolution_clock::now();
    double delta = std::chrono::duration<double>(now - last).count();
    last = now;

    accumulator += delta;

    enet_loop();
    sdl_poll_loop();
    sdl_loop();
    update_modifier();

    while (accumulator >= TICK_TIME)
    {
      // {TICK_RATE} TPS functions
      update_location();
      send_location();
      accumulator -= TICK_TIME;
    }

    global::frames ++;

    if (global::ttf::input_string.ends_with("\n")) {global::ttf::input_string.clear();}
  }

  return 0;
}

int sdl_poll_loop() {
  using global::sdl::sdl_event;
  while (SDL_PollEvent(&sdl_event)) {
    switch (sdl_event.type) {
      case SDL_EVENT_QUIT: global::running = false; break;
      case SDL_EVENT_WINDOW_RESIZED: {
        SDL_GetWindowSize(global::sdl::window, &global::sdl::window_width, &global::sdl::window_height);
        ui::button_continue.update_location(global::sdl::window_width/2, global::sdl::window_height/2 + 100);
        break;
      }
      case SDL_EVENT_TEXT_INPUT: {
        if (!global::ttf::input_string.ends_with('\n')) {
          global::ttf::input_string += sdl_event.text.text;
        }
        break;
      }
      case SDL_EVENT_KEY_DOWN: {
        if (sdl_event.key.key == SDLK_BACKSPACE) {
          if (!global::ttf::input_string.empty()) {
            global::ttf::input_string.pop_back();
          }
        }
        else if (sdl_event.key.key == SDLK_RETURN) {
          if (!global::ttf::input_string.ends_with("\n")) {
            global::ttf::input_string += "\n";
          }
        }

        else if (sdl_event.key.key == SDLK_T) {
          if (global::chat_open) {
            global::chat_open = false;
          }
          else {
            global::chat_open = true;
          }
        }

        else if (sdl_event.key.key == SDLK_V) {
          if (global::modifier == SDL_KMOD_CTRL) {
            string pasted_text = SDL_GetClipboardText();
            if (pasted_text.find('\n') != string::npos) {
              pasted_text.erase(pasted_text.find('\n'));
            }
            global::ttf::input_string += pasted_text;
          }
        }


        break;
      }

      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        if (global::status_menu == STATUS_MENU_DISCONNECTED_FROM_SERVER) {
          ui::button_continue.handle_event(sdl_event, [](){set_status_menu(STATUS_MENU_WAITING_USER_INPUT_IP);});
        }
      }

      default: break;
    }
  }


  return 0;
}

int sdl_loop() {
  SDL_SetRenderDrawColor(global::sdl::renderer, 0, 0, 0, 255);
  SDL_RenderClear(global::sdl::renderer);

  switch (global::status_connection) {
    case STATUS_CONNECTION_NOT_CONNECTED: {
      switch (global::status_menu) {
        case STATUS_MENU_WAITING_USER_INPUT_NAME: {
          activate_text_input();
          ui::render::ask_new_name();
          ui::update_text_input();
          break;
        }

        case STATUS_MENU_WAITING_USER_INPUT_IP: {
          activate_text_input();
          ui::render::ask_server_ip();
          ui::update_text_input();
          break;
        }

        case STATUS_MENU_DISCONNECTED_FROM_SERVER: {
          deactivate_text_input();
          TTF_SetTextString(ui::text_connection_status, "Disconnected", 0);
          ui::button_continue.render();
          ui::render::connection_status();
          break;
        }

        default: break;
      }
      break;
    }

    case STATUS_CONNECTION_CONNECTING: {
      deactivate_text_input();
      TTF_SetTextString(ui::text_connection_status, "Connecting...", 0);
      ui::render::connection_status();
      break;

    }

    case STATUS_CONNECTION_CONNECTED: {
      deactivate_text_input();
      TTF_SetTextString(ui::text_connection_status, "Connected...", 0);
      ui::render::connection_status();
      break;
    }

    case STATUS_CONNECTION_ENCRYPTING: {
      deactivate_text_input();
      TTF_SetTextString(ui::text_connection_status, "Encrypting...", 0);
      ui::render::connection_status();
      break;

    }

    case STATUS_CONNECTION_ENCRYPTED: {
      switch (global::status_menu) {
        case STATUS_MENU_WAITING_USER_INPUT_REGISTER_PASSWORD: {
          activate_text_input();
          ui::render::ask_register_password();
          ui::update_text_input();
          break;
        }

        case STATUS_MENU_WAITING_USER_INPUT_LOGIN_PASSWORD: {
          activate_text_input();
          ui::render::ask_login_password();
          ui::update_text_input();
          break;
        }
        default: break;
      }
    }

      default: break;
    }

  if (global::status_connection == STATUS_CONNECTION_ENCRYPTED) {
    if (global::status_game == STATUS_GAME_PLAYING) {
      if (!global::chat_open) {
        deactivate_text_input();
      }
      render_players();
    }
  }

  if (global::chat_open) {
    activate_text_input();
    using namespace global::ttf;
    using namespace global::sdl;

    activate_text_input();
    ui::update_text_input();

    int t_width, t_height;
    TTF_GetTextSize(ui::text_input, &t_width, &t_height);
    TTF_DrawRendererText(ui::text_input, roundf(static_cast<float>(window_width - t_width) / 2),
                                     roundf(static_cast<float>(window_height - t_height) / 2) - 250);

    if (input_string == "/ping\n") {
      log_info("ping: " + std::to_string(global::enet::connected_server_peer->roundTripTime));
    }


    if (input_string.ends_with('\n')) {
      global::chat_open = false;
    }
  }

  if (global::config::show_fps) {
    int temp_text_width;

    TTF_GetTextSize(ui::text_fps, &temp_text_width, nullptr);
    TTF_DrawRendererText(ui::text_fps,
      static_cast<float>(global::sdl::window_width - temp_text_width - 30), 10);
  }

  if (global::config::debug) {
    TTF_SetTextString(ui::text_debug,
      ("status_conn: " + std::to_string(global::status_connection) +
      "\nstatus_menu: " + std::to_string(global::status_menu) +
      "\nstatus_game: " + std::to_string(global::status_game)).c_str(), 0);

    TTF_DrawRendererText(ui::text_debug, 30, 10);
  }

  SDL_RenderPresent(global::sdl::renderer);
  if (global::status_menu == STATUS_MENU_WAITING_USER_INPUT_IP ||
      global::status_menu == STATUS_MENU_WAITING_USER_INPUT_NAME ||
      global::status_menu == STATUS_MENU_DISCONNECTED_FROM_SERVER ||
      global::status_menu == STATUS_MENU_WAITING_USER_INPUT_REGISTER_PASSWORD ||
      global::status_menu == STATUS_MENU_WAITING_USER_INPUT_LOGIN_PASSWORD ||
      global::status_menu == STATUS_MENU_CONNECTING
      ) {
    SDL_Delay(16);
  }
  return 0;
}

void activate_text_input() {
  if (global::sdl::text_input_active == false) {
    global::sdl::text_input_active = true;
    SDL_StartTextInput(global::sdl::window);
  }
}

void deactivate_text_input() {
  if (global::sdl::text_input_active == true) {
    global::sdl::text_input_active = false;
    SDL_StopTextInput(global::sdl::window);
  }
}

void set_status_menu(const int status) {
  global::status_menu = status;
}
void set_status_connection(const int status) {
  global::status_connection = status;
}
void set_status_game(const int status) {
  global::status_game = status;
}

void count_frames() {
  Uint32 last_time = SDL_GetTicks();

  while (global::running) {
    Uint32 current_time = SDL_GetTicks();
    Uint32 delta = current_time - last_time;

    if (delta >= 1000) {
      global::fps = static_cast<int>(static_cast<float>(global::frames) / (static_cast<float>(delta) / 1000));
      global::frames = 0;
      last_time = current_time;

      TTF_SetTextString(ui::text_fps, std::to_string(global::fps).c_str(), 0);

    }
  }
}

void update_location() {
  const bool *key_states = SDL_GetKeyboardState(nullptr);

  if (key_states[SDL_SCANCODE_W]) {
    global::main_player.location_y -= 4;
  }

  if (key_states[SDL_SCANCODE_S]) {
    global::main_player.location_y += 4;
  }

  if (key_states[SDL_SCANCODE_A]) {
    global::main_player.location_x -= 4;
  }

  if (key_states[SDL_SCANCODE_D]) {
    global::main_player.location_x += 4;
  }
}
void render_players() {
  SDL_SetRenderDrawColor(global::sdl::renderer, 0, 168, 255, 255);
  for (auto& loop_player : global::online_players) {
    loop_player.second.rect = {loop_player.second.location_x, loop_player.second.location_y, 30, 30};
    SDL_RenderFillRect(global::sdl::renderer, &loop_player.second.rect);
    log_debug(loop_player.second.name + ", " + std::to_string(loop_player.second.location_x));
  }

  SDL_SetRenderDrawColor(global::sdl::renderer, 0, 255, 0, 255);
  global::main_player.rect = {global::main_player.location_x, global::main_player.location_y, 30, 30};
  SDL_RenderFillRect(global::sdl::renderer, &global::main_player.rect);
  log_debug(global::main_player.name + ", " + std::to_string(global::main_player.location_x));

}


void start_sdl() {
  global::sdl::window = SDL_CreateWindow("game", global::sdl::window_width, global::sdl::window_height, SDL_WINDOW_RESIZABLE);
  global::sdl::renderer = SDL_CreateRenderer(global::sdl::window, "direct3d11");

  global::ttf::font = TTF_OpenFont("Archivo-SemiBold.ttf", 40);
  global::ttf::text_engine = TTF_CreateRendererTextEngine(global::sdl::renderer);

  SDL_SetRenderVSync(global::sdl::renderer, SDL_RENDERER_VSYNC_DISABLED);

}

void update_modifier() {

  if (SDL_GetModState() & SDL_KMOD_CTRL) {
    global::modifier = SDL_KMOD_CTRL;
  }
  else if (SDL_GetModState() & SDL_KMOD_SHIFT) {
    global::modifier = SDL_KMOD_SHIFT;
  }
  else {
    global::modifier = 0;
  }
}