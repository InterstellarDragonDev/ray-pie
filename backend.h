#ifndef BACKEND_H
#define BACKEND_H

#include "raylib.h"
#include "raymath.h"
#include "toml.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>



class ITEM {
private:
  Texture2D line_texture = LoadTexture("line.png");

public:
  Vector2 current_position, target_postion;
  std::string title;
  const char* icon_path;
  const char* launch_command;
  std::vector<ITEM> button_array;

  void lerp();
  void render_line();
  std::string get_title();
  ITEM();
  virtual ~ITEM() = default;
};


// miscellaneous -------------------------------
void coords_to_polar(Vector2 coords, float& r, float& theta);
void polar_to_coords(float r, float theta, Vector2& coords);



struct settings_struct {
  float animate_speed = 0.2;
  int radius = 250;
  float rotation_offset = 0.0;
  bool fading = false;
  bool dynamic_movement = false;
  bool centre_pointer = false;
  const char* centre_icon = "";
};

// File Reading & Saving
// ---------------------------------------------------------
int load_toml(toml::table& table);
int load_settings(settings_struct& settings, toml::table tbl);

void load_items(toml::table tbl, std::vector<ITEM>& item_vector);
ITEM load_button(toml::v3::table* tbl);
ITEM load_submenu(toml::v3::table* tbl);

#endif