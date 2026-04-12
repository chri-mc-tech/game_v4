#include "client_textures.h"

#include <raylib.h>
#include <string>
#include <unordered_map>

#include "client_global.h"


namespace textures {
  void load_textures() {
    const char* folder_path = "assets/textures/";

    FilePathList files = LoadDirectoryFiles(folder_path);

    for (unsigned int i = 0; i < files.count; i++) {
      if (IsFileExtension(files.paths[i], ".png")) {

        Texture2D texture = LoadTexture(files.paths[i]);

        GenTextureMipmaps(&texture);

        SetTextureFilter(texture, TEXTURE_FILTER_POINT);

        std::string file_name = GetFileNameWithoutExt(files.paths[i]);

        textures[file_name] = texture;
      }
    }

    UnloadDirectoryFiles(files);

    Mesh cube_mesh = GenMeshCube(1.0f, 1.0f, 1.0f);
    block_model = LoadModelFromMesh(cube_mesh);
  }

  void draw_block(Vector3 position, const std::string& texture_name, Color tint) {
    Texture2D texture;
    if (textures.count(texture_name)) {
      texture = textures[texture_name];
    } else {
      texture = textures["no_texture"];
    }

    block_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    DrawModel(block_model, position, 1.0f, tint);
  }

  void update_filter() {
    for (auto& pair : textures) {
      SetTextureFilter(pair.second, global::texture_filter);
    }
  }
}