#pragma once
#include <raylib.h>
#include <string>

namespace ui {
  class Button {
  public:
    Rectangle rect = {};
    const char *text = nullptr;
    int font_size = 0;
    Vector2 text_size = {};
    void create(int t_x, int t_y, int t_width, int t_height, const char * t_text, int t_font_size);
    void render();
    void update(int t_x, int t_y, int t_width, int t_height);
  };

  void create_all_buttons();

  // buttons

  inline Button button_singleplayer;
  inline Button button_multiplayer;
  inline Button button_direct_connect;
  inline Button button_add_server;
  inline Button button_remove_server;

}