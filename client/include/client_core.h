#pragma once
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN

#include <string>

bool initialize_libraries();
int client_run();
int render();
void activate_text_input();
void deactivate_text_input();
void set_status_menu(int status);
void set_status_connection(int status);
void set_status_game(int status);
void update_location();
void render_players();
void start_graphics();
void update_modifier();