#define RAYGUI_IMPLEMENTATION
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

  item_editor_state editor = init_item_editor();
  settings_editor_state settings_editor = init_settings_editor(&menu);

  while (!WindowShouldClose()) {
    if (check_key_inputs(&menu) == 1) {
      break;
    }

    BeginDrawing();
    if (EDIT_MODE) { ClearBackground(BLACK); }
    else { ClearBackground(BLANK); }


    // drawcall, if button is pressed, 1 is returned and program exits
    input_return i = menu.draw();
    if (i.exit) { break; };
    if ((EDIT_MODE) && (i.item != nullptr)) { set_new_focus(&editor, i.item); }

    // Display Info for Item when in edit mode
    if (EDIT_MODE) {
      if (SETTINGS_EDIT) {settings_editor_gui(&settings_editor, &menu); }
      else {item_editor_gui(&editor, &menu);}
    }

    EndDrawing();
  }

exit_program:
  CloseWindow();
  return 0;
}
