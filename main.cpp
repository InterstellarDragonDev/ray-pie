#include "backend.h"

// ini::IniFile config_file;
settings_struct settings;
toml::table tbl;
std::vector<ITEM> item_array;

void update_postion();
int render_items();

const int window_width = 800;
const int window_height = 800;

int main(void) {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
  InitWindow(window_width, window_height, "ray-pie");
  SetTargetFPS(60);


  load_toml(tbl);
  load_settings(settings, tbl);
  load_items(tbl, item_array);

  for (auto& item : item_array) {
    std::cout << "There is an item with title: " << item.get_title() << std::endl;
    if (item.submenu_array.size() > 0) {
      std::cout << "the following submenus were found:" << std::endl;
      for (auto& j : item.submenu_array) {
        std::cout << j.title << std::endl;
      }
      std::cout << std::endl;
    }
  }

  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_A))
      settings.rotation_offset -= 0.08;
    if (IsKeyDown(KEY_D))
      settings.rotation_offset += 0.08;


    BeginDrawing();
    ClearBackground(BLANK);

    DrawCircle(400, 400, 30, RED);
    update_postion();
    if (render_items() == 1) {
      EndDrawing();
      break;
    }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}



void update_postion() {
  float theta = (2 * PI) / item_array.size();

  for (int i = 0; i < item_array.size(); i++) {


    item_array[i].target_angle = theta * i + PI / item_array.size() + settings.rotation_offset;
    item_array[i].target_radius = settings.radius;
    item_array[i].lerp();

  };
}

int render_items() {
  for (auto i : item_array) {
    if (i.parent != "root") { continue; };
    Vector2 current_position = polar_to_coords(i.current_radius, i.current_angle);

    float theta1, theta2, r1, r2;
    coords_to_polar(GetMousePosition(), r1, theta1);
    coords_to_polar(current_position, r2, theta2);

    // I am doing the extra step of converting to position and then back to polar so that the angle of the item is reduced to -2PI < theta < 2PI
    // else it can be any number and that stuffs up the angle check with the mouse.
    // a silly hack ik but I'm kinda lazy and won't be fixing it :3

    if (abs(theta2 - theta1) < PI / item_array.size()) {
      DrawText(i.title.c_str(), current_position.x, (current_position.y < 450) ? current_position.y - 50 : current_position.y + 50, 22, WHITE);
      if (IsMouseButtonPressed(0)) {
        std::cout << i.title << " has been pressed" << std::endl;
        (i.type == "button") ? i.button_pressed() : i.submenu_pressed();
        return 1;
      }
    }

    //DrawCircle(current_position.x, current_position.y, 25, GREEN);
    i.render_line();
    i.render_icon();
  }
  return 0;
}