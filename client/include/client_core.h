#pragma once
#include <string>

bool initialize_libraries();
int client_run();
int sdl_poll_loop();
int sdl_loop();
void activate_text_input();
void deactivate_text_input();
void set_status(int status);
bool config_exist();
void create_config_file();
bool load_config();