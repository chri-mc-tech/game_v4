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
void render_players();
void start_graphics();
void update_modifier();
void create_test_cubes();
void render_test_cubes();
int update_window(double dt);
int update_camera(double dt);
void update_player(double dt);
void draw_player_model();
void draw_player_hitbox();
