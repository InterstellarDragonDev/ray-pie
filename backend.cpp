#include "backend.h"

ITEM::ITEM() {
  current_angle = 0;
  target_angle = 0;
  current_radius = 0;
  target_radius = 0;
  nest_level = 0;

};

void ITEM::lerp() {
  current_angle = Lerp(current_angle, target_angle, 0.2);
  current_radius = Lerp(current_radius, target_radius, 0.2);
};

void ITEM::render_line() {
  Vector2 current_position = polar_to_coords(current_radius, current_angle);

  Vector2 diff = Vector2Subtract({ 400, 400 }, current_position);
  float length = Vector2Length(diff) - 40;
  float angle = atan2f(diff.y, diff.x) * RAD2DEG;

  Rectangle src = { 0, 0, (float)line_texture.width, (float)line_texture.height };
  Rectangle dst = { current_position.x, current_position.y, length, 10 };

  Vector2 origin = { 0, 5 };

  DrawTexturePro(line_texture, src, dst, origin, angle, { 255, 255, 255, 50 });
}

std::string ITEM::get_title() {
  return title;
}



void coords_to_polar(Vector2 coords, float& r, float& theta) {
  coords = Vector2Subtract(coords, { 400, 400 });
  r = Vector2Length(coords);
  theta = atan2(coords.y, coords.x);
}

Vector2 polar_to_coords(float r, float theta) {
  return { r * cos(theta) + 400 , r * sin(theta) + 400 };
}