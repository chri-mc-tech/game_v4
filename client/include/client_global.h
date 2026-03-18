#pragma once

#include <fstream>


#include "client_player.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <enet/enet.h>
#include <unordered_map>

using CryptoPP::Integer;
using CryptoPP::SecByteBlock;

namespace global {
  inline int status_connection = 0;
  inline int status_menu = 0;
  inline bool running;
  inline std::unordered_map<string, Player> online_players;
  inline int frames;
  inline int fps;
  inline Integer client_private_key;
  inline Integer client_public_key;
  inline Integer server_public_key;
  inline Integer shared_key;
  inline SecByteBlock encryption_key;
  inline Player main_player;

  inline std::ofstream log_file;

  inline int modifier;

  inline bool chat_open;
  inline bool inv_open;
}

namespace global::enet {
  inline ENetEvent enet_event;
  inline ENetHost* enet_client = nullptr;
  inline ENetPeer* connected_server_peer;
  inline bool is_connected;
}

namespace global::sdl {
  inline SDL_Event sdl_event;
  inline SDL_Window* window;
  inline SDL_Renderer* renderer;
  inline bool text_input_active;

  inline int window_width = 1280;
  inline int window_height = 720;
  inline float render_scale_x = 1.0f;
  inline float render_scale_y = 1.0f;
}

namespace global::ttf {
  inline TTF_Font* font;
  inline TTF_TextEngine* text_engine;

  inline string input_string;

}

namespace global::config {
  inline string uuid;
  inline string name;
  inline bool show_fps;
  inline bool debug;
  inline bool debug_console;
}

enum status {
  //STATUS_WAITING_USER_INPUT_NAME,
  //STATUS_WAITING_USER_INPUT_IP,
  //STATUS_WAITING_USER_INPUT_REGISTER_PASSWORD,
  //STATUS_WAITING_USER_INPUT_LOGIN_PASSWORD,

  //STATUS_CONNECTING,
  //STATUS_ENCRYPTING,
  //STATUS_CHECKING_REGISTER_PASSWORD,
  //STATUS_CHECKING_LOGIN_PASSWORD,

  //STATUS_AUTHENTICATED,

  //STATUS_ERROR_CONNECTING_TO_SERVER,
  //STATUS_DISCONNECTED_FROM_SERVER,
};

enum status_connection {
  STATUS_CONNECTION_NOT_CONNECTED,
  STATUS_CONNECTION_CONNECTING,
  STATUS_CONNECTION_ENCRYPTING,
  STATUS_CONNECTION_ENCRYPTED,
};

enum status_menu {
  STATUS_MENU_WAITING_USER_INPUT_NAME,
  STATUS_MENU_WAITING_USER_INPUT_IP,
  STATUS_MENU_WAITING_USER_INPUT_REGISTER_PASSWORD,
  STATUS_MENU_WAITING_USER_INPUT_LOGIN_PASSWORD,
  STATUS_MENU_CONNECTING,
  STATUS_MENU_DISCONNECTED_FROM_SERVER,
  STATUS_MENU_CONNECTED,

};