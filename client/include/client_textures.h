#pragma once
#include <raylib.h>
#include <string>
#include <unordered_map>

namespace textures {
  inline std::unordered_map<std::string, Texture2D> textures;

  inline Model block_model;

  void load_textures();
  void draw_block(Vector3 position, const std::string& texture_name, Color tint = WHITE);
  void update_filter();
}