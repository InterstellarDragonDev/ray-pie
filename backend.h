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
#include <filesystem> 
#include "json.hpp"
#include "nanosvg.h"
#include "nanosvgrast.h"

using json = nlohmann::json;


// helper function to load SVG as an image
Image LoadSVGImage(const char* filename, int targetWidth);

inline Vector2 MENU_CENTRE = {400, 400};
inline int GLOBAL_SCALE = 1;
inline bool EDIT_MODE = false;

// raylib log level numbers
/*
LOG_ALL:     0 (Display all logs)
LOG_TRACE:   1 (Trace logging, internal use)
LOG_DEBUG:   2 (Debug logging)
LOG_INFO:    3 (Information logging)
LOG_WARNING: 4 (Warning logging)
LOG_ERROR:   5 (Error logging)
LOG_FATAL:   6 (Fatal logging, aborts program)
LOG_NONE:    7
*/
inline int DEBUG_LOG_RAYLIB = 7;
inline bool DEBUG_LOG_RAYPIE = true;
inline bool DEBUG_LOG_SAVE_FILE = false;


/*
ITEM CLASS:
  Storage class for both buttons and submenus
  * All coords saved in polar and converted to rectangular at render time

*/

class ITEM {
private:
  Image line_img = LoadImageFromMemory(
    ".png",              
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
  Texture2D line_texture = LoadTextureFromImage(line_img);
  int icon_size = 60;
  std::string launch_command;
  std::vector<ITEM> submenu_array;
  float rotation = 0.0f;

  void button_pressed();
  void load_textures();

  void lerp();
  void render_line();
  void render_icon(Vector2 centre, Color tint);

  ITEM();
  ~ITEM();
  NLOHMANN_DEFINE_TYPE_INTRUSIVE(ITEM, type, title, rotation, icon_path, icon_size, launch_command, submenu_array)
};


// miscellaneous -------------------------------

void coords_to_polar(Vector2 coords, float& r, float& theta, Vector2 centre);
Vector2 polar_to_coords(float r, float theta, Vector2 centre);


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


/* RADIAL MENU CLASS */
/*
  item_array : could be called "root" vector. It contains every item loaded straight from the config file
  item_nest : contains pointers to a vector of items (e.g submenu or root) 
  submenu_nest : contains pointers to items (the submenu item itself)

  Submenu Loading Logic:
    the latest entry of item_nest is used in the draw function.
    if a submenu is pressed, the submenu_array is appended into the item_nest.
    This means that it will now be the one getting rendered in the draw call.
    additionally the submenu that was pressed (parent) is appended to the submenu_nest.
    This allows the program to sequencially add and remove from this hierachy allowing for any number of nestings to be possible.
    when a submenu is exited, the program removes the last entry in item_nest and thus restoring us to the previous state.
  
  All other functions are kinda self-explainatory
*/

class RadialMenu {

public:
  settings_struct settings;

  std::vector<ITEM> item_array;
  std::vector<std::vector<ITEM>*> item_nest;
  std::vector<ITEM*> submenu_nest;
  float current_rotation = 0;

  int check_mouse_inputs(ITEM& i, int arr_size);
  void update_postion(ITEM& i, float target_angle);
  void render_items(ITEM& i);

  int draw();

  void print_item_information(std::vector<ITEM>& arr);
  RadialMenu();
};


#endif