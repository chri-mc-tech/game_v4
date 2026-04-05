#pragma once

#define NOGDI
#define NOUSER
#define WIN32_LEAN_AND_MEAN

#include <raylib.h>

#include <fstream>

#include "client_player.h"

#include <unordered_map>

using CryptoPP::Integer;
using CryptoPP::SecByteBlock;



enum status_connection {
  STATUS_CONNECTION_NOT_CONNECTED,
  STATUS_CONNECTION_CONNECTED,
  STATUS_CONNECTION_ENCRYPTED,
};

enum status_menu {
  STATUS_MENU_WAITING_USER_INPUT_NAME,
  STATUS_MENU_MAIN_MENU,
  STATUS_MENU_SINGLEPLAYER,
  STATUS_MENU_MULTIPLAYER,
  STATUS_MENU_DIRECT_CONNECT,
  STATUS_MENU_WAITING_USER_INPUT_IP,
  STATUS_MENU_CONNECTING,
  STATUS_MENU_WAITING_USER_INPUT_PASSWORD,
  STATUS_MENU_IN_GAME,
  STATUS_MENU_DISCONNECTED_FROM_SERVER,
  STATUS_MENU_VOID,
};

enum status_game {
  STATUS_GAME_NONE,
  STATUS_GAME_PLAYING,
  STATUS_GAME_PAUSED
};

enum debug_menu {
  DEBUG_MENU_CLOSED,
  DEBUG_MENU_DEFAULT,
  DEBUG_MENU_ADVANCED
};

class Block {
public:
  int x = 0;
  int y = 0;
  int z = 0;
  bool solid = false;
};

namespace global {
  inline int status_connection = STATUS_CONNECTION_NOT_CONNECTED;
  inline int status_menu = STATUS_MENU_WAITING_USER_INPUT_NAME;
  inline int status_game = STATUS_GAME_NONE;
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
  inline int debug_menu;

  // inline bool is_third_person = false;
  inline float player_height = 1.8f; // 1.8
  inline float player_width = 0.6f; // 0.6
  inline float camera_height = 1.6f; // 1.6

  inline BoundingBox hitbox;

  inline std::vector<Block> world;

  // physics variables
  inline float speed = 2.0f;
  inline float velocityY = 0.0f;
  inline constexpr float GRAVITY = -9.81f;
  inline bool isGrounded = false;
  //

  inline string input_string;

  inline std::ofstream log_file;

  inline int modifier;

  inline bool chat_open;
  inline bool inv_open;

  inline double delta_time;

  inline Vector3 last_movement;
  inline Vector3 next_movement;


  // debug
  static Color cube_colors[50][50];
  //

  // background test
  inline Texture2D background;

}

namespace global::enet {
  inline ENetHost* enet_client = nullptr;
  inline ENetPeer* connected_server_peer;
  inline std::chrono::time_point<std::chrono::steady_clock> start_connection_time;
}

namespace global::graphics {
  inline int window_width = 1280;
  inline int window_height = 720;

  inline float render_scale_x = 1.0f;
  inline float render_scale_y = 1.0f;

  inline Font font;

  inline Camera3D camera = { 0 };
}
