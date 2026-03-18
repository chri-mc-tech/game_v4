#pragma once
#include <string>

bool initialize_libraries();
int client_run();
int sdl_poll_loop();
int sdl_loop();
void activate_text_input();
void deactivate_text_input();
void set_status_menu(int status);
void set_status_connection(int status);
void count_frames();
void update_location();
void render_players();
void start_sdl();
void update_modifier();