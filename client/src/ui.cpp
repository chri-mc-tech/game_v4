#include "ui.h"

#include <charconv>

#include "client_global.h"
#include "client_network.h"

namespace ui {
  // texts:
  // set string and color

  // static shapes:
  // set location and dimension

  // dynamic shapes:
  // nothing


  // at game start
  void create_objects_to_render() {
    using namespace global::ttf;

    text_ask_server_ip = TTF_CreateText(text_engine, font, "Server ip:", 0);
    TTF_SetTextColor(text_ask_server_ip, 255, 255, 255, 255);

    text_connection_status = TTF_CreateText(text_engine, font, NULL, 0);
    TTF_SetTextColor(text_connection_status, 255, 255, 255, 255);
  }

  void create_buttons() {
    shape_continue_button = {static_cast<float>(global::sdl::window_width)/2 - 80, static_cast<float>(global::sdl::window_height)/2 + 30, 160, 50};
    continue_on_error_connecting_to_server.rect = &shape_continue_button;
    continue_on_error_connecting_to_server.target_status = STATUS_WAITING_IP_INPUT;
  }


  // every tick (only if used)
  void update_text_input() {
    using namespace global::ttf;
    using namespace global::sdl;
    using namespace global::enet;

    TTF_DestroyText(text_input);

    text_input = TTF_CreateText(text_engine, font, input_string.c_str(), 0);

  }

}



// every tick
// texts:
// set location and render

// shapes:
// only render
namespace ui::render {

  void ask_server_ip() {
    using namespace global::ttf;
    using namespace global::sdl;
    using namespace global::enet;

    int t_width;
    int t_height;

    TTF_GetTextSize(text_ask_server_ip, &t_width, &t_height);
    TTF_DrawRendererText(text_ask_server_ip, (window_width / 2) - (t_width / 2), (window_height / 2) - (t_height / 2) - 300);


    TTF_GetTextSize(text_input, &t_width, &t_height);
    TTF_DrawRendererText(text_input, (window_width / 2) - (t_width / 2), (window_height / 2) - (t_height / 2) - 250);

    if (input_string.ends_with("\n")) {
      input_string.erase(input_string.length() - 1, 1);

      if (input_string.find(":") == std::string::npos) {connect_to_server(input_string);}
      else {connect_to_server(input_string.substr(0, input_string.find(":")), input_string.substr(input_string.find(":") + 1));}

      input_string.clear();
    }
  }

  void connection_status() {
    using namespace global::ttf;
    using namespace global::sdl;
    using namespace global::enet;

    int t_width;
    int t_height;

    TTF_GetTextSize(text_connection_status, &t_width, &t_height);
    TTF_DrawRendererText(text_connection_status, (window_width / 2) - (t_width / 2), (window_height / 2) - (t_height / 2));
  }
}


//TODO: fare che il pulsante, alla creazione, chiede dimensioni e testo
//genera direttamente lui la forma e il testo, e al rendering la posizione
class Button {
public:
  float loc_x;
  float loc_y;
  float button_width;
  float button_height;
  int text_width;
  int text_height;
  SDL_Color color;
  string text_string;
  SDL_FRect rect;
  TTF_Text *text;

  // call once
  void create_button();

  // call every tick in sdl polling loop in case: SDL_EVENT_MOUSE_BUTTON_DOWN checking current status/ui
  void handle_event(const SDL_Event &event, void (*function_to_execute)()) const;

  // call every tick in sdl loop checking current status/ui
  void render() const;
};

void Button::create_button() {
  rect = {loc_x, loc_y, button_width, button_height};
  text = TTF_CreateText(global::ttf::text_engine, global::ttf::font, text_string.c_str(), 0);
  TTF_GetTextSize(text, &text_width, &text_height);
}

// call every tick in sdl polling loop in case: SDL_EVENT_MOUSE_BUTTON_DOWN
void Button::handle_event(const SDL_Event &event, void (*function_to_execute)()) const {
  if (event.button.button == SDL_BUTTON_LEFT) {
    int mouse_x = event.button.x;
    int mouse_y = event.button.y;

    if (mouse_x >= loc_x && mouse_x <= (loc_x + button_width) && mouse_y >= loc_y && mouse_y <= (loc_y + button_height)) {
      function_to_execute();
    }

  }
}

// call every tick in sdl loop checking current status/ui
void Button::render() const {
  SDL_SetRenderDrawColor(global::sdl::renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(global::sdl::renderer, &rect);
  TTF_DrawRendererText(text, (button_width - text_width) / 2, (button_height - text_height) / 2);

}