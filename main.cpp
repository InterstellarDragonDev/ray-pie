#include "backend.h"

settings_struct settings;

std::vector<ITEM> item_array;
std::vector<std::vector<ITEM>*> item_nest;
std::vector<ITEM*> submenu_nest;
float current_rotation = 0;

int check_mouse_inputs(ITEM& i, int arr_size);
void update_postion(ITEM& i, float target_angle);
void render_items(ITEM& i);
void print_item_information(std::vector<ITEM>& arr);
void cleanup_memory(std::vector<ITEM>& i);

const int window_width = 800;
const int window_height = 800;

// TODO: add svg support, add support for loading, add a default image for buttons and submenu's, 

int main(int argc, char* argv[]) {
  if (argc > 1) {
    if (std::string_view (argv[1]) == "edit") {
      // Enter the edit state from here
    }
  }
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
  InitWindow(window_width, window_height, "ray-pie");
  SetTargetFPS(60);

  SaveConfig save = get_config_file();
  item_array = save.item_vector;
  settings = save.settings;

  for (ITEM &i : item_array) {i.load_textures();};


  item_nest.push_back(&item_array);

  current_rotation = settings.rotation_offset;

  // move cursor to centre of screen
  SetMousePosition(MENU_CENTRE.x, MENU_CENTRE.y);



  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_A)) { current_rotation -= 0.08; std::cout << current_rotation << std::endl; }
    if (IsKeyDown(KEY_D)) { current_rotation += 0.08; std::cout << current_rotation << std::endl; }

    BeginDrawing();
    ClearBackground(BLANK);

    // get all items currently needing to be rendered
    auto focused_item_arr = item_nest.back();

    float theta = (2 * PI) / focused_item_arr->size();

    // loop through the currently focused items and update position + render
    for (int i = 0; i < focused_item_arr->size(); i++) {

      float target_angle = theta * i + PI / focused_item_arr->size() + current_rotation;
      int input_return = check_mouse_inputs((*focused_item_arr)[i], focused_item_arr->size());

      if (input_return == 1) goto  exit_program;
      if (input_return == 2) break;

      update_postion((*focused_item_arr)[i], target_angle);


      render_items((*focused_item_arr)[i]);
    }

    DrawCircle(MENU_CENTRE.x, MENU_CENTRE.y, 30, RED);


    EndDrawing();
  }

exit_program:
  CloseWindow();
  return 0;
}

int check_mouse_inputs(ITEM& i, int arr_size) {

  //std::cout << "Beginning Input Check" << std::endl;

  Vector2 current_position = polar_to_coords(i.current_radius, i.current_angle, MENU_CENTRE);
  float icon_scale = 1.0f;

  float mouse_theta, item_theta, mouse_radius, item_radius;
  coords_to_polar(GetMousePosition(), mouse_radius, mouse_theta, MENU_CENTRE);
  coords_to_polar(current_position, item_radius, item_theta, MENU_CENTRE);

  // I am doing the extra step of converting to position and then back to polar so that the angle of the item is reduced to -2PI < theta < 2PI
  // else it can be any number and that stuffs up the angle check with the mouse.
  // a silly hack ik but I'm kinda lazy and won't be fixing it :3


// check if an Item has been pressed and calls the appropriate function
  if (abs(item_theta - mouse_theta) < PI / arr_size && mouse_radius > settings.radius / 2) {
    DrawText(i.title.c_str(), current_position.x, (current_position.y < 450) ? current_position.y - 50 : current_position.y + 50, 22, WHITE);

    icon_scale = 1.1f;

    if (IsMouseButtonPressed(0)) {

      if (i.type == "button") {
        i.button_pressed();
        return 1;
      }
      // if submenu has been pressed
      std::cout << "moved to submenu " << i.title << std::endl;
      item_nest.push_back(&i.submenu_array);
      submenu_nest.push_back(&i);
      current_rotation = i.rotation;
      SetMousePosition(MENU_CENTRE.x, MENU_CENTRE.y);
      return 2;
    }
  }

  // responsible for closes a submenu or exiting the program if a mouse click happens near the centre
  if (mouse_radius < settings.radius / 2 && IsMouseButtonPressed(0)) {
    // if the item_nest only has one element (that being root) then we can exit if the centre is pressed
    if (item_nest.size() == 1) {
      std::cout << "the exit button has been pressed..." << std::endl;
      return 1;
    }
    else {
      // removing the last item in the nest will push us back to the previously selected item
      item_nest.pop_back();
      submenu_nest.pop_back();
      current_rotation = (submenu_nest.size() == 0) ? settings.rotation_offset : submenu_nest.back()->rotation;

      std::cout << "moved up the nest stack" << std::endl;

      // this ensures that the items get animated when exiting a submenu
      auto temp = item_nest.back();
      for (int j = 0; j < temp->size(); j++) {
        (*temp)[j].current_radius = 0;
        (*temp)[j].current_angle = 0;
      }
      return 2;
    }
  }

  i.icon_size = (int)Lerp(i.icon_size, icon_scale * GLOBAL_SCALE * 60, 0.2);

  //std::cout << "Input Checking Has Been Complete" << std::endl;
  return 0;

}



void update_postion(ITEM& i, float target_angle) {
  //std::cout << "Beginning Position Update" << std::endl;
  i.target_angle = target_angle;
  i.target_radius = settings.radius;


  Vector2 current_position = polar_to_coords(i.current_radius, i.current_angle, MENU_CENTRE);
  float mouse_theta, item_theta, mouse_radius, item_radius;
  coords_to_polar(GetMousePosition(), mouse_radius, mouse_theta, MENU_CENTRE);
  coords_to_polar(current_position, item_radius, item_theta, MENU_CENTRE);
  i.target_radius += (1 - abs(abs(item_theta) - abs(mouse_theta))) * Vector2Distance({ MENU_CENTRE.x, MENU_CENTRE.y}, GetMousePosition()) / 20;

  i.lerp();



  if (i.submenu_array.size() > 0) {

    float phi = (2 * PI) / i.submenu_array.size();

    for (int j = 0; j < i.submenu_array.size(); j++) {
      i.submenu_array[j].target_angle = phi * j;
      i.submenu_array[j].target_radius = 55;
      i.submenu_array[j].lerp();
    }
  };
  //std::cout << "Position Update Complete" << std::endl;

}
// main function that loops through all items and renders them
void render_items(ITEM& i) {
  //std::cout << "Beginning Rendering" << std::endl;
  Vector2 current_position = polar_to_coords(i.current_radius, i.current_angle, MENU_CENTRE);

  i.render_line();
  i.render_icon(MENU_CENTRE, WHITE);

  // render the submenu icons around the submenu as a preview
  if (i.submenu_array.size() > 0) {
    for (auto& j : i.submenu_array) {
      j.icon_size = 18;
      j.render_icon(current_position, { 255, 255, 255, 50 });
    }
  }
  //std::cout << "Rendering Complete." << std::endl;
}


void print_item_information(std::vector<ITEM>& arr) {

  for (auto& item : arr) {
    std::cout << "There is an item with title: " << item.get_title() << std::endl;
    if (item.submenu_array.size() > 0) {
      std::cout << "the following submenus were found:" << std::endl;
      for (auto& j : item.submenu_array) {
        std::cout << j.title << std::endl;
      }
      std::cout << std::endl;
    }
  }
}
