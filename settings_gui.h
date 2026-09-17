/*******************************************************************************************
*
*   LayoutName v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_LAYOUT_NAME_IMPLEMENTATION
*       #include "gui_layout_name.h"
*
*       INIT: GuiLayoutNameState state = InitGuiLayoutName();
*       DRAW: GuiLayoutName(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()

#ifndef GUI_LAYOUT_NAME_H
#define GUI_LAYOUT_NAME_H

typedef struct {
    bool animate_speed_edtEditMode;
    int animate_speed_edtValue;
    bool radius_edtEditMode;
    int radius_edtValue;
    bool rotation_offset_edtEditMode;
    int rotation_offset_edtValue;
    bool icon_scale_edtEditMode;
    int icon_scale_edtValue;
    bool CheckBoxEx004Checked;
    bool centre_pointer_chkChecked;
    bool icon_path_boxEditMode;
    char icon_path_boxText[128];
    bool save_btnPressed;
    bool exit_btnPressed;
    bool update_btnPressed;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiLayoutNameState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiLayoutNameState InitGuiLayoutName(void);
void GuiLayoutName(GuiLayoutNameState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_LAYOUT_NAME_H

/***********************************************************************************
*
*   GUI_LAYOUT_NAME IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_LAYOUT_NAME_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiLayoutNameState InitGuiLayoutName(void)
{
    GuiLayoutNameState state = { 0 };

    state.animate_speed_edtEditMode = false;
    state.animate_speed_edtValue = 0;
    state.radius_edtEditMode = false;
    state.radius_edtValue = 0;
    state.rotation_offset_edtEditMode = false;
    state.rotation_offset_edtValue = 0;
    state.icon_scale_edtEditMode = false;
    state.icon_scale_edtValue = 0;
    state.CheckBoxEx004Checked = false;
    state.centre_pointer_chkChecked = false;
    state.icon_path_boxEditMode = false;
    strcpy(state.icon_path_boxText, "");
    state.save_btnPressed = false;
    state.exit_btnPressed = false;
    state.update_btnPressed = false;

    // Custom variables initialization

    return state;
}

void GuiLayoutName(GuiLayoutNameState *state)
{
    if (GuiValueBox((Rectangle){ 104, 24, 120, 24 }, "Animate Speed ", &state->animate_speed_edtValue, 0, 100, state->animate_speed_edtEditMode)) state->animate_speed_edtEditMode = !state->animate_speed_edtEditMode;
    if (GuiValueBox((Rectangle){ 104, 56, 120, 24 }, "Radius ", &state->radius_edtValue, 0, 100, state->radius_edtEditMode)) state->radius_edtEditMode = !state->radius_edtEditMode;
    if (GuiValueBox((Rectangle){ 104, 128, 120, 24 }, "Rotation Offset ", &state->rotation_offset_edtValue, 0, 100, state->rotation_offset_edtEditMode)) state->rotation_offset_edtEditMode = !state->rotation_offset_edtEditMode;
    if (GuiValueBox((Rectangle){ 104, 160, 120, 24 }, "Icon Scale ", &state->icon_scale_edtValue, 0, 100, state->icon_scale_edtEditMode)) state->icon_scale_edtEditMode = !state->icon_scale_edtEditMode;
    GuiCheckBox((Rectangle){ 104, 152, 24, 24 }, NULL, &state->CheckBoxEx004Checked);
    GuiCheckBox((Rectangle){ 104, 184, 24, 24 }, NULL, &state->centre_pointer_chkChecked);
    if (GuiTextBox((Rectangle){ 104, 216, 328, 24 }, state->icon_path_boxText, 128, state->icon_path_boxEditMode)) state->icon_path_boxEditMode = !state->icon_path_boxEditMode;
    GuiLabel((Rectangle){ 8, 152, 96, 24 }, "Dynamic Movement");
    GuiLabel((Rectangle){ 24, 184, 80, 24 }, "Centre Pointer");
    GuiLabel((Rectangle){ 8, 256, 96, 24 }, "Centre Icon Path");
    state->save_btnPressed = GuiButton((Rectangle){ 24, 304, 96, 32 }, "Save"); 
    state->exit_btnPressed = GuiButton((Rectangle){ 24, 344, 96, 32 }, "Exit"); 
    state->update_btnPressed = GuiButton((Rectangle){ 24, 264, 96, 32 }, "Update"); 
    GuiLine((Rectangle){ 8, 240, 424, 24 }, NULL);
}

#endif // GUI_LAYOUT_NAME_IMPLEMENTATION
