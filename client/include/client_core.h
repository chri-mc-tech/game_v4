#pragma once
#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN

#include <string>

#include "client_ui.h"

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
int update_input();
int update_window();
int update_camera();
void update_player();
void draw_player_model();
void draw_player_hitbox();
bool check_collision();
void rendering_menu();
void rendering_3D();
void get_keyboard_input();
bool is_button_clicked(const ui::Button &button);
void update_player_location(Vector3 t_movement, Vector3 t_rotation);