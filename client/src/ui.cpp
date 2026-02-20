#include "ui.h"

#include "client_global.h"

namespace ui {
  int ask_server_ip() {
    TTF_Text* ask_server_ip_text = TTF_CreateText(global::ttf::text_engine, global::ttf::font, "Server ip:", 0);
    TTF_SetTextColor(ask_server_ip_text, 255, 255, 255, 255);
    int t_width;
    int t_height;
    TTF_GetTextSize(ask_server_ip_text, &t_width, &t_height);
    TTF_DrawRendererText(ask_server_ip_text, (global::sdl::window_width / 2) - (t_width / 2), (global::sdl::window_height / 2) - (t_height / 2) - 50);

    TTF_Text* input_text = TTF_CreateText(global::ttf::text_engine, global::ttf::font, global::ttf::input_text.c_str(), 0);
    TTF_SetTextColor(input_text, 255, 255, 255, 255);
    TTF_GetTextSize(input_text, &t_width, &t_height);
    TTF_DrawRendererText(input_text, (global::sdl::window_width / 2) - (t_width / 2), (global::sdl::window_height / 2) - (t_height / 2));
    if (global::ttf::input_text.ends_with("\n")) {
      global::ttf::input_text.erase(global::ttf::input_text.length() - 1, 1);
      global::server_ip = global::ttf::input_text;
      std::cout << global::ttf::input_text << std::endl;
      std::cout << global::server_ip << std::endl;
      global::ttf::input_text.clear();

    }

    return 0;
  }

}