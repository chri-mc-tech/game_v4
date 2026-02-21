#include "ui.h"

#include <charconv>

#include "client_global.h"
#include "client_network.h"

// call every tick ONLY dynamic text
// static only once
//
// set:
// text to render
// text color

// only create text object

// save static text/ui in ui_cache_text and dynamic in ui_cache_dynamic_text

namespace ui {
  TTF_Text* text_ask_server_ip;
  TTF_Text* text_connecting;
  TTF_Text* text_input;


  // at game start
  void create_text_objects() {
    using namespace global::ttf;

    text_ask_server_ip = TTF_CreateText(text_engine, font, "Server ip:", 0);
    TTF_SetTextColor(text_ask_server_ip, 255, 255, 255, 255);

    text_connecting = TTF_CreateText(text_engine, font, "connecting...", 0);
    TTF_SetTextColor(text_connecting, 255, 255, 255, 255);
    // modify with TTF_SetTextString()

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



// every tick, set location and render
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

  void connecting() {
    using namespace global::ttf;
    using namespace global::sdl;
    using namespace global::enet;

    int t_width;
    int t_height;

    TTF_GetTextSize(text_connecting, &t_width, &t_height);
    TTF_DrawRendererText(text_connecting, (window_width / 2) - (t_width / 2), (window_height / 2) - (t_height / 2));
  }

}
