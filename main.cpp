#include "backend.h"


int window_width = 800;
int window_height = 800;

int main(int argc, char* argv[]) {

  SetTraceLogLevel(DEBUG_LOG_RAYLIB);

  char *window_name = "ray-pie";
  if (argc > 1) {
    if (std::string_view (argv[1]) == "edit") {
      window_width = 1400;
      MENU_CENTRE.x = 1000;
      window_name = "ray-pie configuration";
      EDIT_MODE = true;
      if (DEBUG_LOG_RAYPIE) { std::cout << "Ray-Pie is in Configuration Mode" << std::endl; }
    }
  }
  SetConfigFlags(FLAG_WINDOW_TRANSPARENT);
  InitWindow(window_width, window_height, window_name);
  SetTargetFPS(60);

  RadialMenu menu;


  while (!WindowShouldClose()) {

    if (IsKeyDown(KEY_A)) {  menu.current_rotation -= 0.08; std::cout << menu.current_rotation << std::endl; }
    if (IsKeyDown(KEY_D)) { menu.current_rotation += 0.08; std::cout << menu.current_rotation << std::endl; }

    BeginDrawing();
    ClearBackground(BLANK);

    // drawcall, if button is pressed, 1 is returned and program exits
    if (menu.draw() == 1) {break;};

    EndDrawing();
  }

exit_program:
  CloseWindow();
  return 0;
}

