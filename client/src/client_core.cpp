#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <enet/enet.h>
#include <fstream>
#include <thread>
#include <yaml-cpp/yaml.h>

#include "client_ui.h"
#include "shared_crypto.h"

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

  global::sdl::window = SDL_CreateWindow("game", global::sdl::window_width, global::sdl::window_height, SDL_WINDOW_RESIZABLE);
  global::sdl::renderer = SDL_CreateRenderer(global::sdl::window, "direct3d11");
  /*
  for (int i = 0; i < SDL_GetNumRenderDrivers(); i++) {
    std::cout << SDL_GetRenderDriver(i) << std::endl;
  }
  */

  global::ttf::font = TTF_OpenFont("Archivo-SemiBold.ttf", 40);
  global::ttf::text_engine = TTF_CreateRendererTextEngine(global::sdl::renderer);

  ui::create_objects_to_render();
  ui::create_buttons();

  if (!global::config::name.empty()) {
    global::status = STATUS_WAITING_USER_INPUT_IP;
  }
  std::jthread thread_count_frames(count_frames);

  while (global::running) {
    enet_loop();
    sdl_poll_loop();
    sdl_loop();

    global::frames ++;

    if (global::ttf::input_string.ends_with("\n")) {global::ttf::input_string.clear();}
    // if (!global::ttf::input_string.empty()) {std::cout << global::ttf::input_string << std::endl;}
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
      case SDL_EVENT_TEXT_INPUT: global::ttf::input_string += sdl_event.text.text; break;
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

        break;
      }

      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        if (global::status == STATUS_ERROR_CONNECTING_TO_SERVER || global::status == STATUS_DISCONNECTED_FROM_SERVER) {
          ui::button_continue.handle_event(sdl_event, [](){set_status(STATUS_WAITING_USER_INPUT_IP);});
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

  if (global::status == STATUS_WAITING_USER_INPUT_NAME) {
    activate_text_input();
    ui::render::ask_new_name();
    ui::update_text_input();

  }

  if (global::status == STATUS_WAITING_USER_INPUT_IP) {
    activate_text_input();
    ui::render::ask_server_ip();
    ui::update_text_input();

  }

  if (global::status == STATUS_CONNECTING) {
    deactivate_text_input();
    TTF_SetTextString(ui::text_connection_status, "Connecting...", 0);
    ui::render::connection_status();
  }


  if (global::status == STATUS_ERROR_CONNECTING_TO_SERVER) {
    deactivate_text_input();
    TTF_SetTextString(ui::text_connection_status, "Error connecting to the server", 0);
    ui::button_continue.render();
    ui::render::connection_status();
  }

  if (global::status == STATUS_ENCRYPTING) {
    deactivate_text_input();
    TTF_SetTextString(ui::text_connection_status, "Encrypting", 0);
    ui::render::connection_status();
  }

  if (global::status == STATUS_DISCONNECTED_FROM_SERVER) {
    deactivate_text_input();
    TTF_SetTextString(ui::text_connection_status, "Disconnected from server", 0);
    ui::button_continue.render();
    ui::render::connection_status();
  }

  if (global::config::show_fps) {
    int temp_text_width;

    TTF_GetTextSize(ui::text_fps, &temp_text_width, nullptr);
    TTF_DrawRendererText(ui::text_fps,
      static_cast<float>(global::sdl::window_width - temp_text_width), 0);
  }

  if (global::config::debug) {

  }

  SDL_RenderPresent(global::sdl::renderer);
  if (global::status == STATUS_WAITING_USER_INPUT_IP || global::status == STATUS_WAITING_USER_INPUT_NAME) {
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

void set_status(const int status) {
  global::status = status;
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

      // std::cout << "fps: " << global::fps << std::endl;
      // std::cout << global::status << std::endl;
    }
  }
}

