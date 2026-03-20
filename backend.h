#ifndef BACKEND_H
#define BACKEND_H

#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include "images.h"
#include "json.hpp"
#include "toml.hpp"

using json = nlohmann::json;
/*
ITEM CLASS:
  all coords are saved in polar form and converted for frame to be rendered.


*/


class ITEM {
private:
  Image img = LoadImageFromMemory(
    ".png",              // file type
    line,
    line_length
  );


public:
  float current_radius, target_radius;
  float current_angle, target_angle;
  std::string type;
  std::string title;
  Texture2D icon_texture;
  std::string icon_path;
  Texture2D line_texture = LoadTextureFromImage(img);
  int icon_size = 60;
  std::string launch_command;
  std::vector<ITEM> submenu_array;
  float rotation = 0.0f;

  void submenu_pressed(std::vector<ITEM>& arr);
  void button_pressed();
  void load_textures();

  void lerp();
  void render_line();
  void render_icon(Vector2 centre, Color tint);
  std::string get_title();
  ITEM();
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(ITEM, type, title, rotation, icon_path, icon_size, launch_command, submenu_array)
};


// miscellaneous -------------------------------

void coords_to_polar(Vector2 coords, float& r, float& theta, Vector2 centre);
Vector2 polar_to_coords(float r, float theta, Vector2 centre);

const Vector2 CENTRE = { 400, 400 };

struct settings_struct {
  float animate_speed = 0.2;
  int radius = 250;
  float rotation_offset = 0.0;
  float icon_scale = 1.0;
  bool dynamic_movement = false;
  bool centre_pointer = false;
  std::string centre_icon = "";
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(settings_struct, animate_speed, radius, rotation_offset, icon_scale, dynamic_movement, centre_pointer, centre_icon)
};

class SaveConfig {
public:
  settings_struct settings;
  std::vector<ITEM> item_vector;
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(SaveConfig, settings, item_vector)
};

// File Reading & Saving
// ---------------------------------------------------------
SaveConfig get_config_file();


#endif