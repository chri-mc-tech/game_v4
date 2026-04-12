#include "client_ui.h"

#include "client_global.h"
namespace ui {
  void Button::create(int t_width, int t_height, const char *t_text, int t_font_size) {
    rect = {0, 0, static_cast<float>(t_width), static_cast<float>(t_height)};
    text = t_text;
    text_size = MeasureTextEx(global::graphics::font, text, t_font_size, 0.5);
    font_size = t_font_size;
  }

  void Button::render(int t_x, int t_y) {

    rect = {static_cast<float>(t_x), static_cast<float>(t_y), rect.width, rect.height};

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

    button_singleplayer.create(200, 40, "Singleplayer", 32);

    button_multiplayer.create(200, 40, "Multiplayer", 32);

    button_add_server.create(250, 40, "Add server", 32);

    button_remove_server.create(250, 40, "Remove server", 32);

    button_direct_connect.create(250, 40, "Direct connect", 32);

    button_continue.create(250, 40, "Continue", 32);

    button_back.create(150, 40, "Back", 32);

    button_resume.create(250, 40, "Resume", 32);

    button_settings.create(250, 40, "Settings", 32);

    button_quit.create(150, 40, "Quit", 32);

  }

  void draw_centered_text(const string &text, int x, int y, Color color) {
    using namespace global::graphics;

    Vector2 size = MeasureTextEx(font, text.c_str(), 32, 0.5);

    DrawTextEx(font, text.c_str(),
  {x - (size.x / 2), y - (size.y / 2)},
  32, 0.5, color);
  }

}
