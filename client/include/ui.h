#pragma once
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

#include "client_global.h"

// TODO: fare che al rendering del pulsante venga risettata la posizione riprendendo window_width e window_height
// aggiungere che si puo mettere BUTTON_CENTERED, BUTTON_CENTERED_HORIZONTAL, BUTTON_CENTERED_VERTICAL
// decidere se basarlo su padding o sulla dimensione del pulsante

class Button {
public:
  float loc_x;
  float loc_y;
  float button_width;
  float button_height;
  int padding_left, padding_right, padding_top, padding_bottom;
  int text_width;
  int text_height;
  SDL_Color color;
  string text_string;
  SDL_FRect rect;
  TTF_Text *text;

  // call once
  void create_button();

  // call every tick in sdl polling loop in case: SDL_EVENT_MOUSE_BUTTON_DOWN checking current status/ui
  void handle_event(const SDL_Event &event, std::function<void()> function_to_execute) const;

  // call every tick in sdl loop checking current status/ui
  void render();

  // call every tick if you want to update the location dynamically
  void update_location(float new_loc_x, float new_loc_y);
};

namespace ui {
  inline TTF_Text* text_ask_server_ip;
  inline TTF_Text* text_connection_status;
  inline TTF_Text* text_input;

  void create_objects_to_render();
  void update_text_input();
  void create_buttons();

  // inline Button continue_on_error_connecting_to_server;

  inline Button button_continue;

}

namespace ui::render {
  void ask_server_ip();
  void connection_status();
  void continue_button();

}

enum button_alignment {
  BUTTON_CENTERED = 1,
  BUTTON_CENTERED_HORIZONTAL = 2,
  BUTTON_CENTERED_VERTICAL = 3
};