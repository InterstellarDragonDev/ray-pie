#include "backend.h"

item_editor_state init_item_editor(void) {
    item_editor_state state = { 0 };

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
    state.focus_item = nullptr;

    return state;
}

settings_editor_state init_settings_editor(RadialMenu *menu) {
    settings_editor_state state = { 0 };

    state.animate_speed_edtEditMode = false;
    state.animate_speed_edtValue = menu->settings.animate_speed * 10;
    state.radius_edtEditMode = false;
    state.radius_edtValue = menu->settings.radius;
    state.rotation_offset_edtEditMode = false;
    state.rotation_offset_edtValue = menu->settings.rotation_offset * 180 / PI;
    state.icon_scale_edtEditMode = false;
    state.icon_scale_edtValue = menu->settings.icon_scale * 100;
    state.dynamic_movement_EditMode = false;
    state.dynamic_movement_Active = menu->settings.dynamic_movement;
    state.centre_pointer_chkChecked = menu->settings.centre_pointer;
    state.icon_path_boxEditMode = false;
    strcpy(state.icon_path_boxText, menu->settings.centre_icon.c_str());

    return state;
}

void set_new_focus(item_editor_state *state, ITEM* focus_item) {
    strcpy(state->title_boxText, focus_item->title.c_str());
    strcpy(state->launch_command_boxText, focus_item->launch_command.c_str());
    strcpy(state->icon_path_boxText, focus_item->icon_path.c_str());
    state->icon_size_spnValue = focus_item->icon_size;
    state->submenu_rotation_spnValue = focus_item->rotation * 180 / PI;

    state->focus_item = focus_item;
}

ITEM add_new_item() {
    ITEM i;
    i.title = "New Item";
    i.type = "button";
    return i;
}

// Button Functions --------------------------------------------------------------------------------------------------------------

void update_button(item_editor_state *state) {
    if (state->focus_item == nullptr) {
        std::cout << "\033[31mNo item is currently in focus\033[0m" << std::endl;
        return;
    }
    std::cout << "Updating" << std::endl;
    state->focus_item->title = state->title_boxText;
    if (state->focus_item->icon_path != state->icon_path_boxText) {
        // TODO: Add check to make sure the icon is a valid path
        std::cout << "Icon Path has been changed, loading new icon image to memory." << std::endl;
        state->focus_item->icon_path = state->icon_path_boxText;
        state->focus_item->load_textures();
    }
    // TODO: Input validation
    // TODO: Convert rotations to degrees and vice versa
    state->focus_item->launch_command = state->launch_command_boxText;
    state->focus_item->icon_size = state->icon_size_spnValue;
    state->focus_item->rotation = state->submenu_rotation_spnValue * PI / 180;

    popups.new_popup(4, "Update", "Item Has Been Updated Successfully");
}

// TODO: probably worth adding some visual feedback that the files been saved
void save_button(RadialMenu *menu) {
    save_config_file(&menu->settings, &menu->item_array);
    // menu->reset();
    // menu->init();
    menu->reload_textures();
    popups.new_popup(4, "Save", "Config File Saved Successfully");
}

 // TODO: Figure out what causes the textures to fuck themselves :(
 //* Ok so I "fixed" it by just force reloading the textures cuz I can't be asked to do this properly :>
 // (this will have absolutely no future repurcussions btw, I promise...)
void new_item_button(RadialMenu *menu) {
    if (menu->submenu_nest.size() == 0) {
        std::cout << "Adding new item to [root]" << std::endl;
        menu->item_array.push_back(add_new_item());
    } else {
        auto current_parent = menu->submenu_nest.back();
        std::cout << "Adding new item to: [" << current_parent->title << "]" << std::endl;
        current_parent->submenu_array.push_back(add_new_item());
    }
    menu->reload_textures();
    //TODO: probably need to clear and remake the item_nest cuz the pointers might not be pointing anymore
}

void new_item_focused_button(item_editor_state *state, RadialMenu *menu) {
    if (state->focus_item == nullptr) {
        std::cout << "\033[31mNo item is currently in focus\033[0m" << std::endl;
        return;
    }
    state->focus_item->submenu_array.push_back(add_new_item());
    state->focus_item->type = "sub-menu";
    std::cout << "a new item has been created as a child of [" <<  state->focus_item->title << "]" << std::endl;
    menu->reload_textures();
}

void remove_item_button(item_editor_state *state, RadialMenu *menu) {
    if (state->focus_item == nullptr) {
        std::cout << "\033[31mNo item is currently in focus\033[0m" << std::endl;
        return;
    }
    ITEM *temp = state->focus_item;
    std::string title = state->focus_item->title;
    std::erase_if(*menu->item_nest.back(), [temp](const ITEM& item) { return &item == temp; } );
    //std::erase_if(menu->item_array, [temp](const ITEM& item) { return &item == temp; } );
    state->focus_item = nullptr;
    std::cout << "The item [" << title << "] has been removed" << std::endl;
    menu->reload_textures();
    // if we removed from a submenu, we need to check if anything is left in that submenu
    // if there isn't we need to change it to be a button type and then exit the nesting
    if (menu->submenu_nest.size() == 0) {
        return; // not in a submenu, no further action needed
    }
    if (menu->submenu_nest.back()->submenu_array.size() > 0) {
        return; // submenu has items still in it, no further action needed
    }
    menu->submenu_nest.back()->type = "button"; // no longer a sub-menu, we then move out of the nesting
    std::cout << "\033[36mThe parent submenu [" << menu->submenu_nest.back()->title << "] no longer has any items, therefore no longer classed as a submenu\033[0m" << std::endl;
    menu->item_nest.pop_back(); 
    menu->submenu_nest.pop_back();
    menu->current_rotation = (menu->submenu_nest.size() == 0) ? menu->settings.rotation_offset : menu->submenu_nest.back()->rotation;
    menu->reload_textures();
}

void settings_button() {
    SETTINGS_EDIT = true;
}

void exit_settings_button() {
    SETTINGS_EDIT = false;
}

void update_settings_button(settings_editor_state *state, RadialMenu *menu) {
    menu->settings.animate_speed = (float) state->animate_speed_edtValue / 10.0;
    menu->settings.radius = state->radius_edtValue;
    menu->settings.rotation_offset = (float) state->rotation_offset_edtValue * PI / 180;
    menu->current_rotation = menu->settings.rotation_offset;
    menu->settings.icon_scale = (float) state->icon_scale_edtValue / 100.0;
    menu->settings.dynamic_movement = static_cast<dynamic_movement_enum>(state->dynamic_movement_Active);
    menu->settings.centre_pointer = state->centre_pointer_chkChecked;
    menu->settings.centre_icon = state->icon_path_boxText;

    popups.new_popup(4, "Update Settings", "Settings Have Been Updated Successfully");
}

// GUI Functions ----------------------------------------------------------------------------------------------------------------

void item_editor_gui(item_editor_state *state, RadialMenu *menu) {
    if (GuiTextBox((Rectangle){ 136, 40, 512, 32 }, state->title_boxText, 128, state->title_boxEditMode)) state->title_boxEditMode = !state->title_boxEditMode;
    if (GuiTextBox((Rectangle){ 136, 88, 512, 32 }, state->icon_path_boxText, 128, state->icon_path_boxEditMode)) state->icon_path_boxEditMode = !state->icon_path_boxEditMode;
    if (GuiTextBox((Rectangle){ 136, 136, 512, 32 }, state->launch_command_boxText, 128, state->launch_command_boxEditMode)) state->launch_command_boxEditMode = !state->launch_command_boxEditMode;
    GuiLabel((Rectangle){ 8, 40, 120, 32 }, "Title");
    GuiLabel((Rectangle){ 8, 88, 120, 32 }, "Icon Path");
    GuiLabel((Rectangle){ 8, 136, 120, 32 }, "Launch Command");
    if (GuiSpinner((Rectangle){ 136, 192, 160, 24 }, NULL, &state->icon_size_spnValue, 1, 300, state->icon_size_spnEditMode)) state->icon_size_spnEditMode = !state->icon_size_spnEditMode;
    GuiLabel((Rectangle){ 8, 192, 120, 24 }, "Icon Size");
    GuiLabel((Rectangle){ 8, 232, 120, 24 }, "Submenu Rotation");
    if (GuiButton((Rectangle){ 8, 384, 136, 40 }, "Save New Config")) save_button(menu); 
    GuiLine((Rectangle){ 8, 264, 640, 16 }, NULL);
    if (GuiButton((Rectangle){ 8, 336, 136, 40 }, "Add Item To Current")) new_item_focused_button(state, menu); 
    if (GuiSpinner((Rectangle){ 136, 232, 160, 24 }, NULL, &state->submenu_rotation_spnValue, 0, 365, state->submenu_rotation_spnEditMode)) state->submenu_rotation_spnEditMode = !state->submenu_rotation_spnEditMode;
    if (GuiButton((Rectangle){ 8, 288, 136, 40 }, "Update")) update_button(state); 
    GuiLabel((Rectangle){ 320, 192, 144, 24 }, "In Pixels (Default 60)");
    GuiLabel((Rectangle){ 320, 232, 144, 24 }, "In Degrees (Default 0)");
    GuiLabel((Rectangle){ 168, 288, 352, 40 }, "Click to update the pie menu with the latest information");
    GuiLabel((Rectangle){ 168, 336, 376, 40 }, "Adds an Item to the submenu of the currently selected item");
    GuiLabel((Rectangle){ 168, 384, 448, 16 }, "Saves Changes to the config file. Ensure that you have pressed");
    GuiLabel((Rectangle){ 168, 408, 416, 16 }, "'Update' before pressing this or latest changes won't be saved");
    GuiLine((Rectangle){ 8, 440, 640, 16 }, NULL);
    if (GuiButton((Rectangle){ 8, 536, 144, 48 }, "Settings")) settings_button(); 
    if (GuiButton((Rectangle){ 160, 536, 152, 48 }, "Add New Item")) new_item_button(menu); 
    if (GuiButton((Rectangle){ 318, 536, 152, 48 }, "Remove Current Item")) remove_item_button(state, menu); 
}

void settings_editor_gui(settings_editor_state *state, RadialMenu *menu) {
    if (GuiValueBox((Rectangle){ 104, 24, 120, 24 }, "Animate Speed ", &state->animate_speed_edtValue, 0, 10, state->animate_speed_edtEditMode)) state->animate_speed_edtEditMode = !state->animate_speed_edtEditMode;
    if (GuiValueBox((Rectangle){ 104, 56, 120, 24 }, "Radius ", &state->radius_edtValue, 10, 1000, state->radius_edtEditMode)) state->radius_edtEditMode = !state->radius_edtEditMode;
    if (GuiValueBox((Rectangle){ 104, 88, 120, 24 }, "Rotation Offset ", &state->rotation_offset_edtValue, 0, 365, state->rotation_offset_edtEditMode)) state->rotation_offset_edtEditMode = !state->rotation_offset_edtEditMode;
    if (GuiValueBox((Rectangle){ 104, 120, 120, 24 }, "Icon Scale ", &state->icon_scale_edtValue, 0, 100, state->icon_scale_edtEditMode)) state->icon_scale_edtEditMode = !state->icon_scale_edtEditMode;
    if (state->dynamic_movement_EditMode) GuiLock();
    if (GuiDropdownBox((Rectangle){ 104, 152, 120, 24}, "NONE;SINGLE;DOUBLE", &state->dynamic_movement_Active, state->dynamic_movement_EditMode)) state->dynamic_movement_EditMode = !state->dynamic_movement_EditMode;
    GuiUnlock();
    GuiCheckBox((Rectangle){ 104, 184, 24, 24 }, NULL, &state->centre_pointer_chkChecked);
    if (GuiTextBox((Rectangle){ 104, 216, 328, 24 }, state->icon_path_boxText, 128, state->icon_path_boxEditMode)) state->icon_path_boxEditMode = !state->icon_path_boxEditMode;
    GuiLabel((Rectangle){ 8, 152, 96, 24 }, "Dynamic Movement");
    GuiLabel((Rectangle){ 24, 184, 80, 24 }, "Centre Pointer");
    GuiLabel((Rectangle){ 8, 216, 96, 24 }, "Centre Icon Path");
    if(GuiButton((Rectangle){ 24, 304, 96, 32 }, "Save")) save_button(menu);; 
    if(GuiButton((Rectangle){ 24, 344, 96, 32 }, "Exit")) exit_settings_button(); 
    if(GuiButton((Rectangle){ 24, 264, 96, 32 }, "Update")) update_settings_button(state, menu); 
    GuiLine((Rectangle){ 8, 240, 424, 24 }, NULL);
}