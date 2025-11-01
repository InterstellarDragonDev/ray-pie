#ifndef BACKEND_H
#define BACKEND_H

#include "raylib.h"
#include "raymath.h"
#include "toml.hpp"
#include <cmath>
#include <iostream>
#include <vector>
#include <memory>

/*
ITEM CLASS:
  all coords are saved in polar form and converted for frame to be rendered.


*/


class ITEM {
private:
  Texture2D line_texture = LoadTexture("line.png");

public:
  float current_radius, target_radius;
  float current_angle, target_angle;
  std::string type;
  int nest_level;
  std::string title;
  std::string parent;
  Texture2D icon_texture;
  int icon_size = 60;
  std::string launch_command;
  std::vector<ITEM> submenu_array;

  void submenu_pressed();
  void button_pressed();

  void lerp();
  void render_line();
  void render_icon(Vector2 centre, float scale, Color tint);
  std::string get_title();
  ITEM();
};


// miscellaneous -------------------------------

void coords_to_polar(Vector2 coords, float& r, float& theta, Vector2 centre);
Vector2 polar_to_coords(float r, float theta, Vector2 centre);

const Vector2 CENTRE = { 400, 400 };

struct settings_struct {
  float animate_speed = 0.2;
  int radius = 250;
  float rotation_offset = 0.0;
  bool fading = false;
  bool dynamic_movement = false;
  bool centre_pointer = false;
  std::string centre_icon = "";
};

// File Reading & Saving
// ---------------------------------------------------------
int load_toml(toml::table& table);
int load_settings(settings_struct& settings, toml::table tbl);

void load_items(toml::table tbl, std::vector<ITEM>& item_vector);
ITEM load_button(toml::v3::table* tbl);
ITEM load_submenu(toml::v3::table* tbl);

#endif