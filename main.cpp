#include "backend.h"

// ini::IniFile config_file;
settings_struct settings;
toml::table tbl;
std::vector<ITEM> item_array;
std::vector<ITEM> current_array;
ITEM* current_focus = nullptr;

void update_postion(std::vector<ITEM>& arr);
int render_items(std::vector<ITEM>& arr);

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
    if (current_array.size() > 0) {
      update_postion(current_array);
      if (render_items(current_array) == 1) {
        EndDrawing();
        break;
      }
    }
    else {
      update_postion(item_array);
      if (render_items(item_array) == 1) {
        EndDrawing();
        break;
      }
    }


    EndDrawing();
  }

  CloseWindow();
  return 0;
}



void update_postion(std::vector<ITEM>& arr) {
  float theta = (2 * PI) / arr.size();

  for (int i = 0; i < arr.size(); i++) {


    arr[i].target_angle = theta * i + PI / arr.size() + settings.rotation_offset;
    arr[i].target_radius = settings.radius;
    arr[i].lerp();

    if (arr[i].submenu_array.size() > 0) {

      float phi = (2 * PI) / arr.size();

      for (int j = 0; j < arr[i].submenu_array.size(); j++) {
        arr[i].submenu_array[j].target_angle = phi * j;
        arr[i].submenu_array[j].target_radius = 55;
        arr[i].submenu_array[j].lerp();
      }
    }

  };
}

int render_items(std::vector<ITEM>& arr) {
  bool has_run = false;
  for (auto& i : arr) {
    Vector2 current_position = polar_to_coords(i.current_radius, i.current_angle, CENTRE);
    float icon_scale = 1.0f;

    float theta1, theta2, r1, r2;
    coords_to_polar(GetMousePosition(), r1, theta1, CENTRE);
    coords_to_polar(current_position, r2, theta2, CENTRE);

    // I am doing the extra step of converting to position and then back to polar so that the angle of the item is reduced to -2PI < theta < 2PI
    // else it can be any number and that stuffs up the angle check with the mouse.
    // a silly hack ik but I'm kinda lazy and won't be fixing it :3

    if (abs(theta2 - theta1) < PI / arr.size() && r1 > settings.radius / 2) {
      DrawText(i.title.c_str(), current_position.x, (current_position.y < 450) ? current_position.y - 50 : current_position.y + 50, 22, WHITE);
      icon_scale = 1.1f;
      if (IsMouseButtonPressed(0)) {
        std::cout << i.title << " has been pressed" << std::endl;
        if (i.type == "button") {
          i.button_pressed();
          return 1;
        }

        i.submenu_pressed(current_array);


      }
    }
    // TODO : Make this not register 10 keypresses in a row (it is a slight issue)
    if (r1 < settings.radius / 2 && IsMouseButtonPressed(0) && has_run == false) {
      if (current_array.size() == 0) {
        std::cout << "the exit button has been pressed..." << std::endl;
        return 1;
      }
      else {
        current_array.clear();
        std::cout << "the array is being cleared..." << std::endl;
        // this ensures that the items get animated when exiting a submenu
        for (auto& d : item_array) {
          d.current_radius = 0;
          d.current_angle = 0;
        }
      }
      has_run = true;
    }

    i.render_line();
    i.render_icon(CENTRE, icon_scale, WHITE);

    if (i.submenu_array.size() > 0) {
      for (auto& j : i.submenu_array) {
        j.render_icon(Vector2Add(current_position, { (float)i.icon_size / 2 - 10, (float)i.icon_size / 2 - 10 }), 0.3 * icon_scale, { 255, 255, 255, 50 });
      }
    }
  }
  return 0;
}