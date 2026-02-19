#include "client_core.h"
#include "client_global.h"
#include "client_network.h"

#include <enet/enet.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

int sdl_poll_loop();
int sdl_loop();
int ask_server_ip();

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

  global::sdl::font = TTF_OpenFont("main_font.ttf", 16);

  global::sdl::window = SDL_CreateWindow("game", global::sdl::window_width, global::sdl::window_height, SDL_WINDOW_RESIZABLE);
  global::sdl::renderer = SDL_CreateRenderer(global::sdl::window, nullptr);

  while (global::running) {
    enet_loop();
    sdl_poll_loop();
    sdl_loop();
    ask_server_ip();
  }

  return 0;
}

int sdl_poll_loop() {
  while (SDL_PollEvent(&global::sdl::sdl_event)) {
    switch (global::sdl::sdl_event.type) {
      case SDL_EVENT_QUIT: global::running = false;
      case SDL_EVENT_WINDOW_RESIZED:
      default: break;
    }
  }

  return 0;
}

int sdl_loop() {
  // render objects
  return 0;
}

int ask_server_ip() {
  string t_text = "Server ip:";
  TTF_Text* text = TTF_CreateText(nullptr, global::sdl::font, t_text.c_str(), t_text.length());
  TTF_DrawRendererText(text, (global::sdl::window_width / 2), (global::sdl::window_height / 2));
  return 0;
}