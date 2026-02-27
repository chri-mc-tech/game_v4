#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <enet/enet.h>
#include <yaml-cpp/yaml.h>
#include <fstream>

#include "shared_crypto.h"
#include "client_ui.h"

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
  global::sdl::renderer = SDL_CreateRenderer(global::sdl::window, nullptr);

  global::ttf::font = TTF_OpenFont("Archivo-SemiBold.ttf", 40);
  global::ttf::text_engine = TTF_CreateRendererTextEngine(global::sdl::renderer);

  ui::create_objects_to_render();
  ui::create_buttons();

  Uint32 last_time = SDL_GetTicks();
  int frames = 0;
  float fps = 0.0f;

  while (global::running) {
    enet_loop();
    sdl_poll_loop();
    sdl_loop();

    frames ++;

    Uint32 current_time = SDL_GetTicks();
    Uint32 delta = current_time - last_time;

    if (delta >= 1000) {
      fps = frames / (delta / 1000);
      frames = 0;
      last_time = current_time;

      std::cout << "fps: " << fps << std::endl;
    }


    if (global::ttf::input_string.ends_with("\n"))  {global::ttf::input_string.clear();}
    if (!global::ttf::input_string.empty()) {std::cout << global::ttf::input_string << std::endl;}
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
        ui::button_continue.handle_event(sdl_event, [](){set_status(STATUS_WAITING_USER_INPUT_IP);});
      }

      default: break;
    }
  }


  return 0;
}

int sdl_loop() {
  SDL_SetRenderDrawColor(global::sdl::renderer, 0, 0, 0, 255);
  SDL_RenderClear(global::sdl::renderer);
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

  std::cout << ui::button_continue.loc_x << ", " << ui::button_continue.loc_y << std::endl;
  std::cout << ui::button_continue.text_width << ", " << ui::button_continue.text_height << std::endl;

  SDL_RenderPresent(global::sdl::renderer);
  // SDL_Delay(2);
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

bool config_exist() {
  using namespace YAML;

  try {
    Node config = LoadFile("config.yaml");
  } catch (const BadFile&) {
    return false;
  }
  return true;
}

void create_config_file() {
  using namespace YAML;
  using std::ofstream;

  Node config;
  config["uuid"] = shared::crypto::generate_uuid();
  config["name"] = "";
  ofstream fout("config.yaml");

  fout << "# Configuration file \n";
  fout << "# all server data are saved based on uuid \n";
  fout << "# DO NOT CHANGE THE UUID MANUALLY \n";

  fout << config;
  fout.close();
  std::cout << "config file created" << std::endl;

}

bool load_config() {
  using namespace YAML;

  Node config = LoadFile("config.yaml");
  config::uuid = config["uuid"].as<string>();
  config::name = config["name"].as<string>();

  return true;
}

string ask_new_name() {
  //todo: fare nuovo stato per input nome
}