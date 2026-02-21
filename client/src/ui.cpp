#include "ui.h"

#include <charconv>

#include "client_global.h"
#include "client_network.h"

namespace ui {
  int ask_server_ip() {
    using namespace global::ttf;
    using namespace global::sdl;
    using namespace global::enet;

    TTF_Text* ask_server_ip_text = TTF_CreateText(text_engine, font, "Server ip:", 0);
    TTF_SetTextColor(ask_server_ip_text, 255, 255, 255, 255);
    int t_width;
    int t_height;
    TTF_GetTextSize(ask_server_ip_text, &t_width, &t_height);
    TTF_DrawRendererText(ask_server_ip_text, (window_width / 2) - (t_width / 2), (window_height / 2) - (t_height / 2) - 50);

    TTF_Text* t_input_text = TTF_CreateText(text_engine, font, input_text.c_str(), 0);
    TTF_SetTextColor(t_input_text, 255, 255, 255, 255);
    TTF_GetTextSize(t_input_text, &t_width, &t_height);
    TTF_DrawRendererText(t_input_text, (window_width / 2) - (t_width / 2), (window_height / 2) - (t_height / 2));
    if (input_text.ends_with("\n")) {
      input_text.erase(input_text.length() - 1, 1);

      if (input_text.find(":") == std::string::npos) {connect_to_server(input_text);}
      else {connect_to_server(input_text.substr(0, input_text.find(":")), input_text.substr(input_text.find(":") + 1));}

      input_text.clear();
    }

    return 0;
  }


  int send_loading_screen_message() {
    using namespace global::ttf;
    using namespace global::sdl;
    using namespace global::enet;

    TTF_Text* text = TTF_CreateText(text_engine, font, loading_screen_text.c_str(), 0);
    TTF_SetTextColor(text, 255, 255, 255, 255);
    int t_width;
    int t_height;
    TTF_GetTextSize(text, &t_width, &t_height);
    TTF_DrawRendererText(text, (window_width / 2) - (t_width / 2), (window_height / 2) - (t_height / 2) - 50);

    return 0;
  }




}