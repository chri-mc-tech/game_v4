#pragma once

#include "client_player.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <enet/enet.h>
#include <unordered_map>

namespace global {
  inline bool debug = true;
  inline bool running;
  inline std::unordered_map<string, player> online_players;
}

namespace global::enet {
  inline ENetEvent enet_event;
  inline ENetHost* enet_client = nullptr;
}

namespace global::sdl {
  inline SDL_Event sdl_event;
  inline SDL_Window* window;
  inline SDL_Renderer* renderer;

  inline int window_width = 1280;
  inline int window_height = 720;
  inline float render_scale_x = 1.0f;
  inline float render_scale_y = 1.0f;
}

namespace global::ttf {
  inline TTF_Font* font;
  inline TTF_TextEngine* text_engine;

  inline string input_text;

}

namespace config {}