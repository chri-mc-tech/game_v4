#pragma once
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "client_global.h"

class Button {
public:
  SDL_FRect *rect;
  int target_status;

  void handle_event(const SDL_Event &event) const;

  static void render(SDL_FRect rect);
};

namespace ui {
  inline TTF_Text* text_ask_server_ip;
  inline TTF_Text* text_connection_status;
  inline TTF_Text* text_input;

  inline SDL_FRect shape_continue_button;

  void create_objects_to_render();
  void update_text_input();
  void create_buttons();

  inline Button continue_on_error_connecting_to_server;

}

namespace ui::render {
  void ask_server_ip();
  void connection_status();
  void continue_button();

}

