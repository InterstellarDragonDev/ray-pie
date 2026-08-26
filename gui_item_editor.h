#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <cstring>
#include <iostream>

#ifndef GUI_ITEM_EDITOR_H
#define GUI_ITEM_EDITOR_H

typedef struct {
    bool title_boxEditMode;
    char title_boxText[128];
    bool icon_path_boxEditMode;
    char icon_path_boxText[256];
    bool launch_command_boxEditMode;
    char launch_command_boxText[256];
    bool icon_size_spnEditMode;
    int icon_size_spnValue;
    bool submenu_rotation_spnEditMode;
    int submenu_rotation_spnValue;

    // function references for buttons
    void *menu;

    void (*save)(void*);
    void (*add_to_current)();
    void (*update)();
    void (*settings)();
    void (*new_item)();

} GuiItemEditorState;


//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiItemEditorState InitGuiItemEditor(void);
void GuiItemEditor(GuiItemEditorState *state);


#endif // GUI_ITEM_EDITOR_H

/***********************************************************************************
*
*   GUI_ITEM_EDITOR IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_ITEM_EDITOR_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiItemEditorState InitGuiItemEditor(void)
{
    GuiItemEditorState state = { 0 };

    state.title_boxEditMode = false;
    strcpy(state.title_boxText, "Title");
    state.icon_path_boxEditMode = false;
    strcpy(state.icon_path_boxText, "Icon Path");
    state.launch_command_boxEditMode = false;
    strcpy(state.launch_command_boxText, "Launch Command");
    state.icon_size_spnEditMode = false;
    state.icon_size_spnValue = 0;
    state.submenu_rotation_spnEditMode = false;
    state.submenu_rotation_spnValue = 0;

    // Custom variables initialization

    return state;
}


void GuiItemEditor(GuiItemEditorState *state)
{
    if (GuiTextBox((Rectangle){ 136, 40, 512, 32 }, state->title_boxText, 128, state->title_boxEditMode)) state->title_boxEditMode = !state->title_boxEditMode;
    if (GuiTextBox((Rectangle){ 136, 88, 512, 32 }, state->icon_path_boxText, 128, state->icon_path_boxEditMode)) state->icon_path_boxEditMode = !state->icon_path_boxEditMode;
    if (GuiTextBox((Rectangle){ 136, 136, 512, 32 }, state->launch_command_boxText, 128, state->launch_command_boxEditMode)) state->launch_command_boxEditMode = !state->launch_command_boxEditMode;
    GuiLabel((Rectangle){ 8, 40, 120, 32 }, "Title");
    GuiLabel((Rectangle){ 8, 88, 120, 32 }, "Icon Path");
    GuiLabel((Rectangle){ 8, 136, 120, 32 }, "Launch Command");
    if (GuiSpinner((Rectangle){ 136, 192, 160, 24 }, NULL, &state->icon_size_spnValue, 0, 100, state->icon_size_spnEditMode)) state->icon_size_spnEditMode = !state->icon_size_spnEditMode;
    GuiLabel((Rectangle){ 8, 192, 120, 24 }, "Icon Size");
    GuiLabel((Rectangle){ 8, 232, 120, 24 }, "Submenu Rotation");
    if (GuiButton((Rectangle){ 8, 384, 136, 40 }, "Save New Config")) state->save(state->menu); 
    GuiLine((Rectangle){ 8, 264, 640, 16 }, NULL);
    if (GuiButton((Rectangle){ 8, 336, 136, 40 }, "Add Item To Current")) state->add_to_current(); 
    if (GuiSpinner((Rectangle){ 136, 232, 160, 24 }, NULL, &state->submenu_rotation_spnValue, 0, 100, state->submenu_rotation_spnEditMode)) state->submenu_rotation_spnEditMode = !state->submenu_rotation_spnEditMode;
    if (GuiButton((Rectangle){ 8, 288, 136, 40 }, "Update")) state->update(); 
    GuiLabel((Rectangle){ 320, 192, 144, 24 }, "In Pixels (Default 60)");
    GuiLabel((Rectangle){ 320, 232, 144, 24 }, "In Degrees (Default 0)");
    GuiLabel((Rectangle){ 168, 288, 352, 40 }, "Click to update the pie menu with the latest information");
    GuiLabel((Rectangle){ 168, 336, 376, 40 }, "Adds an Item to the submenu of the currently selected item");
    GuiLabel((Rectangle){ 168, 384, 448, 16 }, "Saves Changes to the config file. Ensure that you have pressed");
    GuiLabel((Rectangle){ 168, 408, 416, 16 }, "'Update' before pressing this or latest changes won't be saved");
    GuiLine((Rectangle){ 8, 440, 640, 16 }, NULL);
    if (GuiButton((Rectangle){ 8, 536, 144, 48 }, "Settings")) state->settings(); 
    if (GuiButton((Rectangle){ 160, 536, 152, 48 }, "Add New Item")) state->new_item(); 
}

#endif // GUI_ITEM_EDITOR_IMPLEMENTATION
