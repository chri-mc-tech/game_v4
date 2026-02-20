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

  SDL_StartTextInput(global::sdl::window);

  global::ttf::font = TTF_OpenFont("main_font.ttf", 32);
  global::ttf::text_engine = TTF_CreateRendererTextEngine(global::sdl::renderer);

  while (global::running) {
    enet_loop();
    sdl_poll_loop();
    sdl_loop();

    if (global::ttf::input_text.ends_with("\n"))  {global::ttf::input_text.clear();}
    if (!global::ttf::input_text.empty()) {std::cout << global::ttf::input_text << std::endl;}

  }

  return 0;
}

int sdl_poll_loop() {
  while (SDL_PollEvent(&global::sdl::sdl_event)) {
    switch (global::sdl::sdl_event.type) {
      case SDL_EVENT_QUIT: global::running = false; break;
      case SDL_EVENT_WINDOW_RESIZED: SDL_GetWindowSize(global::sdl::window, &global::sdl::window_width, &global::sdl::window_height); break;
      case SDL_EVENT_TEXT_INPUT: global::ttf::input_text += global::sdl::sdl_event.text.text; break;
      case SDL_EVENT_KEY_DOWN: {
        if (!global::ttf::input_text.empty()) {
          if (global::sdl::sdl_event.key.key == SDLK_BACKSPACE) {
            global::ttf::input_text.pop_back();
          }
          else if (global::sdl::sdl_event.key.key == SDLK_RETURN) {
            if (!global::ttf::input_text.ends_with("\n")) {
              global::ttf::input_text += "\n";
            }
          }
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
  if (global::server_ip.empty()) {
    ui::ask_server_ip();
  }
  else {
    //TODO: fare che, dato ip:porta in input viene tentato il collegamento al server
  }

  SDL_RenderPresent(global::sdl::renderer);
  SDL_Delay(10);
  return 0;
}



