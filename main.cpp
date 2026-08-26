#include "backend.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define GUI_ITEM_EDITOR_IMPLEMENTATION
#include "gui_item_editor.h"

int window_width = 800;
int window_height = 800;

void test_func() {
  std::cout << "greetings tarnished" << std::endl;
}

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

  GuiItemEditorState editorState = InitGuiItemEditor();

  editorState.menu = &menu;
  editorState.save = save_button;
  editorState.add_to_current = test_func;
  editorState.update = test_func;
  editorState.settings = test_func;
  editorState.new_item = test_func;

  std::cout << "\033[36mSomehow got to here\033[0m" << std::endl;

  while (!WindowShouldClose()) {

    if (check_key_inputs(&menu) == 1) {
      break;
    }

    BeginDrawing();
    if (EDIT_MODE) { ClearBackground(BLACK); }
    else { ClearBackground(BLANK); }


    // drawcall, if button is pressed, 1 is returned and program exits
    if (menu.draw() == 1) {break;};

    // Display Info for Item when in edit mode
    if (EDIT_MODE) {
      GuiItemEditor(&editorState);
    }

    EndDrawing();
  }

exit_program:
  CloseWindow();
  return 0;
}
