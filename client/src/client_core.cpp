#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <enet/enet.h>

#include "ui.h"

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

  global::ttf::font = TTF_OpenFont("main_font.ttf", 32);
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
      case SDL_EVENT_WINDOW_RESIZED: SDL_GetWindowSize(global::sdl::window, &global::sdl::window_width, &global::sdl::window_height); break;
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
      }

      case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        ui::continue_on_error_connecting_to_server.handle_event(sdl_event);
      }

      default: break;
    }
  }


  return 0;
}

int sdl_loop() {
  SDL_SetRenderDrawColor(global::sdl::renderer, 0, 0, 0, 255);
  SDL_RenderClear(global::sdl::renderer);
  if (global::status == STATUS_WAITING_IP_INPUT) {
    activate_text_input();
    ui::render::ask_server_ip();
    ui::update_text_input();

  }

  if (global::status == STATUS_WAITING_TO_CONNECT) {
    deactivate_text_input();
    TTF_SetTextString(ui::text_connection_status, "Connecting...", 0);
    ui::render::connection_status();
  }


  if (global::status == STATUS_ERROR_CONNECTING_TO_SERVER) {
    deactivate_text_input();
    TTF_SetTextString(ui::text_connection_status, "Error connecting to the server \n\n\n                   continue", 0);
    // temp
    ui::shape_continue_button = {static_cast<float>(global::sdl::window_width)/2 - 80, static_cast<float>(global::sdl::window_height)/2 + 30, 160, 50};
    Button::render(ui::shape_continue_button);
    ui::render::connection_status();

  }

  if (global::status == STATUS_CONNECTED_TO_SERVER) {
    TTF_SetTextString(ui::text_connection_status, "Connected", 0);
    ui::render::connection_status();
  }

  if (global::status == STATUS_DISCONNECTED_FROM_SERVER) {
    TTF_SetTextString(ui::text_connection_status, "Disconnected from server \n\n\n                   continue", 0);
    // temp
    ui::shape_continue_button = {static_cast<float>(global::sdl::window_width)/2 - 80, static_cast<float>(global::sdl::window_height)/2 + 30, 160, 50};
    Button::render(ui::shape_continue_button);
    ui::render::connection_status();
  }


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