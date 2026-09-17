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
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include <algorithm>

#include "raygui.h"

using json = nlohmann::json;


// helper function to load SVG as an image
Image LoadSVGImage(const char* filename, int targetWidth);

inline Vector2 MENU_CENTRE = {400, 400};
inline int GLOBAL_SCALE = 7;
inline bool EDIT_MODE = false;
inline bool SETTINGS_EDIT = false;

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
inline int DEBUG_LOG_RAYLIB = 5;
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
  int target_icon_size = 60;
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


// miscellaneous ------------------------------------------------------------------------

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
void save_config_file(settings_struct *settings, std::vector<ITEM> *item_arr);


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

struct input_return {
  ITEM* item;
  bool exit;
};


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

  input_return draw();

  void print_item_information(std::vector<ITEM>& arr);
  RadialMenu();
  void reset();
  void init();
  void reload_textures();
};


std::filesystem::path get_executable_path();
int check_key_inputs(RadialMenu *menu);


/* Edit Mode */
// ------------------------------------------------------------------------------------------------------

typedef struct {
    bool title_boxEditMode;
    char title_boxText[128];
    bool icon_path_boxEditMode;
    char icon_path_boxText[256];
    bool launch_command_boxEditMode;
    char launch_command_boxText[256];
    bool icon_size_spnEditMode;
    int icon_size_spnValue;
    bool submenu_rotation_spnEditMode;
    int submenu_rotation_spnValue;

    ITEM* focus_item;
    // function references for buttons
    void (*settings)();

} item_editor_state;

typedef struct {
    bool animate_speed_edtEditMode;
    int animate_speed_edtValue;
    bool radius_edtEditMode;
    int radius_edtValue;
    bool rotation_offset_edtEditMode;
    int rotation_offset_edtValue;
    bool icon_scale_edtEditMode;
    int icon_scale_edtValue;
    bool dynamic_movement_chkChecked;
    bool centre_pointer_chkChecked;
    bool icon_path_boxEditMode;
    char icon_path_boxText[128];
    bool save_btnPressed;
    bool exit_btnPressed;
    bool update_btnPressed;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} settings_editor_state;

// init gui states
item_editor_state init_item_editor(void);
settings_editor_state init_settings_editor(RadialMenu *menu);


void set_new_focus(item_editor_state *state, ITEM* focus_item);

// Button Functions
void update_button(item_editor_state *state);
void save_button(RadialMenu *menu);
void new_item_button(RadialMenu *menu);
void new_item_focused_button(item_editor_state *state, RadialMenu *menu);
void remove_item_button(item_editor_state *state, RadialMenu *menu);
void settings_button();

void exit_settings_button();
void update_settings_button(settings_editor_state *state, RadialMenu *menu);

// renders the gui
void item_editor_gui(item_editor_state *state, RadialMenu *menu);
void settings_editor_gui(settings_editor_state *state, RadialMenu *menu);


#endif