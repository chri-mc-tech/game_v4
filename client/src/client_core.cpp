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

  global::sdl::window = SDL_CreateWindow("game", global::sdl::window_width, global::sdl::window_height, SDL_WINDOW_RESIZABLE);
  global::sdl::renderer = SDL_CreateRenderer(global::sdl::window, nullptr);

  SDL_StartTextInput(global::sdl::window);

  global::ttf::font = TTF_OpenFont("main_font.ttf", 32);
  global::ttf::text_engine = TTF_CreateRendererTextEngine(global::sdl::renderer);

  while (global::running) {
    enet_loop();
    sdl_poll_loop();
    sdl_loop();
    std::cout << global::ttf::input_text << std::endl;
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
            global::ttf::input_text.clear();
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
  ask_server_ip();

  SDL_RenderPresent(global::sdl::renderer);
  SDL_Delay(10);
  return 0;
}

int ask_server_ip() {
  TTF_Text* ask_server_ip_text = TTF_CreateText(global::ttf::text_engine, global::ttf::font, "Server ip:", 0);
  TTF_SetTextColor(ask_server_ip_text, 255, 255, 255, 255);
  int t_width;
  int t_height;
  TTF_GetTextSize(ask_server_ip_text, &t_width, &t_height);
  TTF_DrawRendererText(ask_server_ip_text, (global::sdl::window_width / 2) - (t_width / 2), (global::sdl::window_height / 2) - (t_height / 2) - 100);


  TTF_Text* input_text = TTF_CreateText(global::ttf::text_engine, global::ttf::font, global::ttf::input_text.c_str(), 0);
  TTF_SetTextColor(input_text, 255, 255, 255, 255);
  TTF_GetTextSize(input_text, &t_width, &t_height);
  TTF_DrawRendererText(input_text, (global::sdl::window_width / 2) - (t_width / 2), (global::sdl::window_height / 2) - (t_height / 2));

  return 0;
}