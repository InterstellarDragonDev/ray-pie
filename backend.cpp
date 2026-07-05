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
  std::cout << "unloaded " << title << "'s texture successfully" << std::endl;
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
  Vector2 origin = { icon_size / 2, icon_size / 2 };

  Rectangle dst = { current_position.x, current_position.y,  icon_size, icon_size };

  DrawTexturePro(icon_texture, src, dst, origin, 0, tint);
}

void ITEM::load_textures() {
  // if the image can't be found the default image must be loaded
  std::filesystem::path path = icon_path.c_str();
  if (!std::filesystem::exists(path)) {
    std::cout << "Item: " << title << " failed to load image: " << icon_path << " as it does not exist. Using default icon instead" << std::endl;
    Image img;
    if (submenu_array.size() > 0) {img = LoadImageFromMemory(".png", folder_icon, folder_icon_length);}
    else {img = LoadImageFromMemory(".png", app_icon, app_icon_length);}
    icon_texture = LoadTextureFromImage(img);
  }

  else {
    if (path.extension() == ".svg") {
      std::cout << "svg detected" << std::endl;
      Image img = LoadSVGImage(icon_path.c_str(), 128);
      icon_texture = LoadTextureFromImage(img);
    }
    else {
      icon_texture = LoadTexture(icon_path.c_str());
    }
  }

  for (ITEM &i : submenu_array) {i.load_textures();};
}

std::string ITEM::get_title() {
  return title;
}

void ITEM::button_pressed() {
  std::cout << "A button has been pressed! " << std::endl;
  std::string cmd = "nohup " + launch_command + " >/dev/null 2>&1 &";
  std::system(cmd.c_str());
}

void ITEM::submenu_pressed(std::vector<ITEM>& arr) {
  arr = submenu_array;
  std::cout << "A submenu has been pressed!" << std::endl;
}



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

   if (!svg)
{
    TraceLog(LOG_ERROR, "Failed to load SVG: %s", filename);
    return { 0 };
}
    // Calculate the scale required to achieve the requested width
    float scale = (float)targetWidth / svg->width;

    int width  = targetWidth;
    int height = (int)(svg->height * scale + 0.5f);

    unsigned char* pixels = new unsigned char[width * height * 4];
    std::cout << "we got here";
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