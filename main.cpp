#include "backend.h"

// ini::IniFile config_file;
settings_struct settings;
toml::table tbl;
std::vector<ITEM> item_array;

void update_postion();
void render_items();

const int window_width = 800;
const int window_height = 800;

int main(void) {
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
  InitWindow(window_width, window_height, "ray-pie");
  SetTargetFPS(60);


  load_toml(tbl);
  load_settings(settings, tbl);
  load_items(tbl, item_array);

  for (auto item : item_array) {
    std::cout << "There is an item with title: " << item.get_title() << std::endl;
  }

  float r, theta;

  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_A))
      settings.rotation_offset -= 0.08;
    if (IsKeyDown(KEY_D))
      settings.rotation_offset += 0.08;

    coords_to_polar(GetMousePosition(), r, theta);
    std::cout << "polar coords of mouse cursor is: radius: " << r << " angle: " << theta << std::endl;


    BeginDrawing();
    ClearBackground(BLANK);

    DrawCircle(400, 400, 30, RED);
    update_postion();
    render_items();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}



void update_postion() {
  float theta = (2 * PI) / item_array.size();

  for (int i = 0; i < item_array.size(); i++) {
    float x = window_width / 2 +
      settings.radius *
      cos(theta * i + PI / item_array.size() + settings.rotation_offset);
    float y = window_height / 2 +
      settings.radius *
      sin(theta * i + PI / item_array.size() + settings.rotation_offset);
    item_array[i].target_postion = { x, y };
    item_array[i].lerp();


  };
}

void render_items() {
  for (auto i : item_array) {

    float theta1, theta2, r1, r2;
    coords_to_polar(GetMousePosition(), r1, theta1);
    coords_to_polar(i.target_postion, r2, theta2);

    if (abs(theta2 - theta1) < PI / item_array.size()) {
      DrawText(i.title.c_str(), i.target_postion.x, (i.target_postion.y < 450) ? i.target_postion.y - 50 : i.target_postion.y + 50, 22, WHITE);
    }

    DrawCircle(i.current_position.x, i.current_position.y, 25, GREEN);
    i.render_line();
  }
}