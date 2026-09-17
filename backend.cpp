#define NANOSVG_ALL_COLOR_KEYWORDS	
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#include "backend.h"

ITEM::ITEM() {
  current_angle = 0;
  target_angle = 0;
  current_radius = 0;
  target_radius = 0;
};
ITEM::~ITEM() {
  UnloadTexture(icon_texture);
  if (DEBUG_LOG_RAYPIE) { std::cout << "unloaded [" << title << "] texture successfully" << std::endl; }

}

void ITEM::lerp() {
  current_angle = Lerp(current_angle, target_angle, 0.2);
  current_radius = Lerp(current_radius, target_radius, 0.2);
};

void ITEM::render_line() {
  Vector2 current_position = polar_to_coords(current_radius, current_angle, MENU_CENTRE);

  Vector2 diff = Vector2Subtract(MENU_CENTRE, current_position);
  float length = Vector2Length(diff) - 40;
  float angle = atan2f(diff.y, diff.x) * RAD2DEG;

  Rectangle src = { 0, 0, (float)line_texture.width, (float)line_texture.height };
  Rectangle dst = { current_position.x, current_position.y, length, 10 };

  Vector2 origin = { 0, 5 };

  DrawTexturePro(line_texture, src, dst, origin, angle, { 255, 255, 255, 50 });
}

void ITEM::render_icon(Vector2 centre, Color tint) {
  Vector2 current_position = polar_to_coords(current_radius, current_angle, centre);

  Rectangle src = { 0, 0, (float)icon_texture.width, (float)icon_texture.height };
  Vector2 origin = { target_icon_size / 2, target_icon_size / 2 };

  Rectangle dst = { current_position.x, current_position.y,  target_icon_size, target_icon_size };

  DrawTexturePro(icon_texture, src, dst, origin, 0, tint);
}

void ITEM::load_textures() {
  // if the image can't be found the default image must be loaded
  std::filesystem::path path = icon_path.c_str();
  if (!std::filesystem::exists(path)) {
    if (DEBUG_LOG_RAYPIE) { std::cout << "\033[31mItem : [" << title << "] failed to load image: " << icon_path << " as it does not exist. Using default icon instead\033[0m" << std::endl; }
    Image img;
    if (submenu_array.size() > 0) {img = LoadImageFromMemory(".png", folder_icon, folder_icon_length);}
    else {img = LoadImageFromMemory(".png", app_icon, app_icon_length);}
    icon_texture = LoadTextureFromImage(img);
  }

  else {
    if (path.extension() == ".svg") {
      if (DEBUG_LOG_RAYPIE) { std::cout << "svg detected" << std::endl; }
      Image img = LoadSVGImage(icon_path.c_str(), 128);
      icon_texture = LoadTextureFromImage(img);
    }
    else {
      icon_texture = LoadTexture(icon_path.c_str());
    }
  }

  for (ITEM &i : submenu_array) {i.load_textures();};
}

void ITEM::button_pressed() {
  if (DEBUG_LOG_RAYPIE) { std::cout << "button [" << title << "] has been pressed" << std::endl; }

  if (launch_command == "") {
    if (DEBUG_LOG_RAYPIE) { std::cout << "\033[31mNo launch command was provided for this button\033[0m" << std::endl; }
    return;
  }
  if (DEBUG_LOG_RAYPIE) {std::cout << "launching command: " << launch_command << std::endl; }

  std::string cmd = "nohup " + launch_command + " >/dev/null 2>&1 &";
  std::system(cmd.c_str());
}


// miscellaneous
//--------------------------------------------------------------------------------------------------------------------

void coords_to_polar(Vector2 coords, float& r, float& theta, Vector2 centre) {
  coords = Vector2Subtract(coords, centre);
  r = Vector2Length(coords);
  theta = atan2(coords.y, coords.x);
}

Vector2 polar_to_coords(float r, float theta, Vector2 centre) {
  return { r * cos(theta) + centre.x , r * sin(theta) + centre.y };
}



Image LoadSVGImage(const char* filename, int targetWidth)
{
    NSVGimage* svg = nsvgParseFromFile(filename, "px", 96.0f);

   if (!svg) {
    if (DEBUG_LOG_RAYPIE) { std::cout << "\033[31mFailed to load SVG: " << filename  << "\033[0m" << std::endl; }
    return { 0 };
}
    // Calculate the scale required to achieve the requested width
    float scale = (float)targetWidth / svg->width;

    int width  = targetWidth;
    int height = (int)(svg->height * scale + 0.5f);

    unsigned char* pixels = new unsigned char[width * height * 4];
    NSVGrasterizer* rast = nsvgCreateRasterizer();

    nsvgRasterize(
        rast,
        svg,
        0.0f,
        0.0f,
        scale,
        pixels,
        width,
        height,
        width * 4
    );

    Image image = {
        pixels,
        width,
        height,
        1,
        PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };

    nsvgDeleteRasterizer(rast);
    nsvgDelete(svg);

    return image;
}

// RadialMenu
// -----------------------------------------------------------------------------------------------------------

RadialMenu::RadialMenu() {
  init();
}

void RadialMenu::reset() {
  item_nest.clear();
  submenu_nest.clear();
  item_array.clear();
}

void RadialMenu::init() {
  SaveConfig save = get_config_file();
  item_array = save.item_vector;
  settings = save.settings;
  if (DEBUG_LOG_RAYPIE) { std::cout << "Ignore Texture Unloading Above, its part of the config file loading process" << std::endl << std::endl; }
  for (ITEM &i : item_array) {i.load_textures();};
  item_nest.push_back(&item_array);
  current_rotation = settings.rotation_offset;

  if (DEBUG_LOG_SAVE_FILE) print_item_information(item_array);

  SetMousePosition(MENU_CENTRE.x, MENU_CENTRE.y);
}

input_return RadialMenu::draw() {
   // get all items currently needing to be rendered
    auto focused_item_arr = item_nest.back();

    float theta = (2 * PI) / focused_item_arr->size();

    // loop through the currently focused items and update position + render
    for (int i = 0; i < focused_item_arr->size(); i++) {

      float target_angle = theta * i + PI / focused_item_arr->size() + current_rotation;
      int input_return = check_mouse_inputs((*focused_item_arr)[i], focused_item_arr->size());

      if (input_return == 1) return {nullptr, true};
      if (input_return == 2) return {nullptr, false};
      if (input_return == 3) return {&(*focused_item_arr)[i], false};
      

      update_postion((*focused_item_arr)[i], target_angle);


      render_items((*focused_item_arr)[i]);
    }

    DrawCircle(MENU_CENTRE.x, MENU_CENTRE.y, 30, RED);
    return {nullptr, false}; // return nothing if no action is needed
  
}

int RadialMenu::check_mouse_inputs(ITEM& i, int arr_size) {

  

  Vector2 current_position = polar_to_coords(i.current_radius, i.current_angle, MENU_CENTRE);
  float icon_scale = 1.0f;

  float mouse_theta, item_theta, mouse_radius, item_radius;
  coords_to_polar(GetMousePosition(), mouse_radius, mouse_theta, MENU_CENTRE);
  coords_to_polar(current_position, item_radius, item_theta, MENU_CENTRE);

  // I am doing the extra step of converting to position and then back to polar so that the angle of the item is reduced to -2PI < theta < 2PI
  // else it can be any number and that stuffs up the angle check with the mouse.
  // a silly hack ik but I'm kinda lazy and won't be fixing it :3

  if (EDIT_MODE) {
        if (mouse_radius > settings.radius + 80) { return 0; }
  }

// check if an Item has been pressed and calls the appropriate function
  if (abs(item_theta - mouse_theta) < PI / arr_size && mouse_radius > settings.radius / 2) {
    DrawText(i.title.c_str(), current_position.x, (current_position.y < 450) ? current_position.y - 50 : current_position.y + 50, 22, WHITE);

    icon_scale = 1.1f;

    // to navigate into a submenu during edit mode, right click must be used
    if (IsMouseButtonPressed(1)) {
      if (EDIT_MODE) { goto do_submenu; }
    }

    if (IsMouseButtonPressed(0)) {

      if (EDIT_MODE) {
        return 3;
      }

      if (i.type == "button") {
        i.button_pressed();
        return 1;
      }

      do_submenu:
      // if submenu has been pressed
      if (i.type == "sub-menu") {
        if (DEBUG_LOG_RAYPIE) { std::cout << "moved to submenu [" << i.title << "]" << std::endl; }
        item_nest.push_back(&i.submenu_array);
        submenu_nest.push_back(&i);
        current_rotation = i.rotation;
        SetMousePosition(MENU_CENTRE.x, MENU_CENTRE.y);
        return 2;
      }
    }
  }

  // responsible for closes a submenu or exiting the program if a mouse click happens near the centre
  if (mouse_radius < settings.radius / 2 && IsMouseButtonPressed(0)) {
    // if the item_nest only has one element (that being root) then we can exit if the centre is pressed
    if (item_nest.size() == 1) {
      if (DEBUG_LOG_RAYPIE) { std::cout << "the exit button has been pressed..." << std::endl; }
      return 1;
    }
    else {
      // removing the last item in the nest will push us back to the previously selected item
      item_nest.pop_back();
      submenu_nest.pop_back();
      current_rotation = (submenu_nest.size() == 0) ? settings.rotation_offset : submenu_nest.back()->rotation;

      if (DEBUG_LOG_RAYPIE) { std::cout << "moved up the nest stack" << std::endl; }

      // this ensures that the items get animated when exiting a submenu
      auto temp = item_nest.back();
      for (int j = 0; j < temp->size(); j++) {
        (*temp)[j].current_radius = 0;
        (*temp)[j].current_angle = 0;
      }
      return 2;
    }
  }

  // TODO: figure out a good icon_scaling equations
    i.target_icon_size = i.icon_size;
  //i.target_icon_size = (int)Lerp(i.icon_size, icon_scale * GLOBAL_SCALE * 60, 0.2);

  return 0;

}



void RadialMenu::update_postion(ITEM& i, float target_angle) {
  i.target_angle = target_angle;
  i.target_radius = settings.radius;


  Vector2 current_position = polar_to_coords(i.current_radius, i.current_angle, MENU_CENTRE);
  float mouse_theta, item_theta, mouse_radius, item_radius;
  coords_to_polar(GetMousePosition(), mouse_radius, mouse_theta, MENU_CENTRE);
  coords_to_polar(current_position, item_radius, item_theta, MENU_CENTRE);
  
  // old dynamic movement system
  // if (settings.dynamic_movement) {
  // i.target_radius += (1 - abs(abs(item_theta) - abs(mouse_theta))) * Vector2Distance({ MENU_CENTRE.x, MENU_CENTRE.y}, GetMousePosition()) / 20;
  // }
  
  // updated dynamic movement system
  if (settings.dynamic_movement) {
    float dot = Vector2DotProduct(Vector2Normalize(Vector2Subtract(current_position, MENU_CENTRE)), Vector2Normalize(Vector2Subtract(GetMousePosition(), MENU_CENTRE)));
    //float dot = (abs(Vector2DotProduct(Vector2Normalize(Vector2Subtract(current_position, MENU_CENTRE)), Vector2Normalize(Vector2Subtract(GetMousePosition(), MENU_CENTRE)))) * 2) - 1;
    float dist = Vector2Distance(MENU_CENTRE, GetMousePosition()); 
    //std::cout << i.title << ":  " << dot << std::endl;
    i.target_radius = settings.radius + dot * dist / 8;
  }

  i.lerp();


  if (i.submenu_array.size() > 0) {

    float phi = (2 * PI) / i.submenu_array.size();

    for (int j = 0; j < i.submenu_array.size(); j++) {
      i.submenu_array[j].target_angle = phi * j;
      i.submenu_array[j].target_radius = 55;
      i.submenu_array[j].lerp();
    }
  };

}
// main function that loops through all items and renders them
void RadialMenu::render_items(ITEM& i) {
  Vector2 current_position = polar_to_coords(i.current_radius, i.current_angle, MENU_CENTRE);

  i.render_line();
  i.render_icon(MENU_CENTRE, WHITE);

  // render the submenu icons around the submenu as a preview
  if (i.submenu_array.size() > 0) {
    for (auto& j : i.submenu_array) {
      j.target_icon_size = 18;
      j.render_icon(current_position, { 255, 255, 255, 50 });
    }
  }
}


void RadialMenu::print_item_information(std::vector<ITEM>& arr) {

  for (auto& item : arr) {
    std::cout << item.title << " : " << item.type << std::endl;
    if (item.submenu_array.size() > 0) {
      for (auto& j : item.submenu_array) {
        std::cout << "├──" << j.title << std::endl;
      }
      std::cout << std::endl;
    }
  }
}

void RadialMenu::reload_textures() {
  if (DEBUG_LOG_RAYPIE) { std::cout << "\033[36mReloading Textures.....\033[0m" << std::endl; }
  for (int i = 0; i < item_array.size(); i++) {
    item_array[i].load_textures();
  }
  if (DEBUG_LOG_RAYPIE) { std::cout << "\033[36mTextures have been Reloaded\033[0m" << std::endl; }
}

// miscellaneous -----------------------------------------------

// gets the current path of the ray-pie executable
std::filesystem::path get_executable_path() {
  char buffer[PATH_MAX];
    // /proc/self/exe is a symlink pointing to the true binary
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        return std::filesystem::path(buffer);
    }
}


// Checks if any keyboard buttons were pressed and handles them accordingly
int check_key_inputs(RadialMenu *menu) {

  // handles the rotation of current menu
  if (IsKeyDown(KEY_MINUS)) {  menu->current_rotation -= 0.01; std::cout << menu->current_rotation * 180 / PI << std::endl; }
  if (IsKeyDown(KEY_EQUAL)) { menu->current_rotation += 0.01; std::cout << menu->current_rotation * 180 / PI << std::endl; }
  
  if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
    // press Ctrl + E to relaunch in Edit mode
    if (IsKeyPressed(KEY_E)) {
      std::filesystem::path full_path = get_executable_path();
      if (full_path.empty()) {std::cout << "\033[31mFailed to detect executable path.\033[0m" << std::endl; }
      if (DEBUG_LOG_RAYPIE) { std::cout << "Path of current executable: " << full_path << std::endl << "\033[36mRelaunching in Config Mode\033[0m" << std::endl; }
      std::string cmd = "nohup " + full_path.string() + " edit " + " >/dev/null 2>&1 &";
      std::system(cmd.c_str());
      return 1;
    }
    // Press Ctrl + R to refresh the menu (reloads the config file)
    else if (IsKeyPressed(KEY_R)) {
      if (DEBUG_LOG_RAYPIE) { std::cout << "\033[36mReloading Radial Menu\033[0m" << std::endl; }
      // reset the radial menu (to clear out any nesting) and then re-initialise it
      menu->reset();
      menu->init();
    }
    // Press Ctrl + T to reload all textures in the menu
    else if (IsKeyPressed(KEY_T)) {
      menu->reload_textures();
    }
  }
  
  if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
    
  }

  return 0;
}