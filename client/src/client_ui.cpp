#include "client_ui.h"

#include "client_global.h"
namespace ui {
  void Button::create(int t_x, int t_y, int t_width, int t_height, const char *t_text, int t_font_size) {
    rect = {static_cast<float>(t_x), static_cast<float>(t_y), static_cast<float>(t_width), static_cast<float>(t_height)};
    text = t_text;
    text_size = MeasureTextEx(global::graphics::font, text, t_font_size, 0.5);
    font_size = t_font_size;
  }

  void Button::render() {
    DrawRectangleRounded(rect, 0.5, 10, GRAY);
    DrawTextEx(global::graphics::font, text,
      {
        rect.x + ((rect.width - text_size.x) / 2),
        rect.y + ((rect.height - text_size.y) / 2)
      },
      font_size, 0.5, WHITE);
  }



  void create_all_buttons() {
    using namespace global::graphics;

    button_singleplayer.create(window_width/2 - 100,
      window_height/2 - 60,
      200, 40, "Singleplayer", 32);

    button_multiplayer.create(window_width/2 - 100,
    window_height/2,
    200, 40, "Multiplayer", 32);


  }
}
