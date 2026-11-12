# ray-pie

It is a lightweight radial launcher for Linux, built with C++ and [raylib](https://www.raylib.com/). Items are arranged in a circle around a central point and launched by moving towards an desired item and clicking. Applications can be organized into nested sub-menus. The launcher can be configured via the inbuilt "edit mode" so there's no need to edit the config file directly.

---

## Features

- Radial menu with animated item positioning
- Nested submenus to any depth (click to open, click the centre to go back)
- Built-in **edit mode**: a GUI for adding, editing, removing and saving items and global settings
- Icon support for raster images types loaded by raylib as well as support for SVG.
- Dynamic mouse-proximity item movement (optional, `SINGLE` / `DOUBLE` modes)

---

## Dependencies

- [raylib](https://www.raylib.com/): windowing, rendering, input (installed on your system)
- [raygui](https://github.com/raysan5/raygui): the edit-mode GUI and popups (`raygui.h`)
- [nlohmann/json](https://github.com/nlohmann/json): config parsing (`json.hpp`)
- [nanosvg](https://github.com/memononen/nanosvg): SVG icon loading (`nanosvg.h`, `nanosvgrast.h`)
- A **C++20**-capable compiler (GCC, Clang). `std::erase_if` on vectors is used
- A compositor supporting transparent windows (e.g. Hyprland, Picom)
- Linux and X11 (the code uses `/proc/self/exe` and `nohup`, and links against `-lX11`)

Notes that all single header Dependencies are included in the repo. Only `raylib` needs to be installed.

---

## Building

Requires `raylib` to be installed on your system.

```bash
# Clone the repo
git clone https://github.com/InterstellarDragonDev/ray-pie.git
cd ray-pie
mkdir build

# Build and run
make
```

This compiles with:

```bash
g++ main.cpp backend.h backend.cpp file_managing.cpp json.hpp nanosvg.h nanosvgrast.h images.h raygui.h gui.cpp -o build/ray-pie -w -lraylib  -lGL -lm -lpthread -ldl -lrt -lX11
```

The binary is output to `build/ray-pie`. Running `make` will also launch it immediately after a successful build.

---

## Usage

Run the binary to open the launcher:

```bash
./ray-pie
```

To open the editor directly:

```bash
./ray-pie edit
```

### Menu controls

| Input | Action |
|-------|--------|
| **Hover** near an item | Highlights it and shows its title |
| **Left click** a `button` item | Runs its launch command and closes the launcher |
| **Left click** a `sub-menu` item | Opens the submenu |
| **Left click** the red centre circle | Goes back up one submenu level, or exits the launcher if you're at the top level |
| **Esc** | Closes the launcher |
| Hold **`-`** / **`=`** | Rotates the current ring counter-clockwise / clockwise (the angle in degrees is printed to the terminal) |
| **Ctrl + E** | Closes the launcher and relaunches it in edit mode |
| **Ctrl + R** | Reloads the config file and resets the menu (discards unsaved changes) |
| **Ctrl + T** | Reloads all item textures |

The cursor is moved to the centre of the menu on launch and whenever you enter a submenu.
Note that you do not need to click the item's icon. The launcher will accpet any click that is in the general direction of the item, so long as its more than half way between the centre and the item.

---

## Edit Mode

Edit mode opens a larger window (1400×800) with the radial menu on the right and the editor panel on the left. Start it with `./ray-pie edit` or press **Ctrl + E** from the normal menu.

### Navigating in edit mode

| Input | Action |
|-------|--------|
| **Left click** an item | Selects it and loads its details into the editor |
| **Right click** a submenu item | Opens that submenu (so you can edit its children) |
| **Left click** the centre circle | Goes back up one level, or exits at the top level |


### Item editor

| Control | Description |
|---------|-------------|
| **Title** | Name shown on hover |
| **Icon Path** | Path to a PNG/SVG/etc icon |
| **Launch Command** | Shell command to run (buttons only) |
| **Icon Size** | Size of the icon in pixels (1–300, default 60) |
| **Submenu Rotation** | Rotation applied when this submenu is opened, in degrees (0–365, default 0) |
| **Update** | Applies the editor fields to the selected item in the live menu |
| **Add Item To Current** | Adds a new child to the selected item and turns it into a `sub-menu` |
| **Save New Config** | Writes the current menu and settings to the config file |
| **Settings** | Opens the settings editor |
| **Add New Item** | Adds a new `button` to the ring you're currently viewing (root or the open submenu) |
| **Remove Current Item** | Deletes the selected item. If this empties a submenu, its parent reverts to a `button` and you're moved back up one level |

> **Note:** press **Update** before **Save New Config**, otherwise your latest changes won't be saved to the config file.

### Settings editor

| Control | Description |
|---------|-------------|
| **Animate Speed** | 0–10 (stored as 0.0–1.0) |
| **Radius** | Distance of items from the centre, 10–1000 |
| **Rotation Offset** | Initial rotation of the wheel, in degrees (0–365) |
| **Icon Scale** | 0–100 (stored as 0.0–1.0) |
| **Dynamic Movement** | `NONE`, `SINGLE` or `DOUBLE` |
| **Centre Pointer** | Checkbox |
| **Centre Icon Path** | Path to a centre icon |
| **Update** | Applies the settings to the live menu |
| **Save** | Writes the config file |
| **Exit** | Returns to the item editor |

---

## Configuration

The config file is located at:

```
~/.config/ray-pie/config.json
```

If the file doesn't exist, a default one (one button, one submenu containing one button) is generated automatically on first launch. The `~/.config` directory must already exist.

Rotation values are stored in **radians** in the file, but are shown in **degrees** in the editor.

### Settings

| Key | Type | Description |
|-----|------|-------------|
| `radius` | int | Distance of items from the centre (default: 250) |
| `rotation_offset` | float | Initial rotation of the wheel in radians (default: 0.0) |
| `dynamic_movement` | int | `0` = none, `1` = single, `2` = double (see below) |
| `dmv_strength` | float | Strength of the dynamic movement effect (default: 1.0). Not exposed in the editor (still working on it) |
| `animate_speed` | float | Intended for lerp speed for item movement (default: 0.2) (not implemented yet) |
| `icon_scale` | float | Intended for global icon scale multiplier (default: 1.0) (not implemented yet) |
| `centre_pointer` | bool | Intended for cursor-centring option (default: false) (not implemented yet) |
| `centre_icon` | string | Intended for path to an icon displayed at the centre (not implemented yet) |


#### Dynamic movement

Items move in or out depending on where the cursor is, scaled by `dmv_strength` (if you really wanna change this just edit the config directly) and by how far the cursor is from the centre.

- **`SINGLE` (1):** items on the same side as the cursor move outward, items on the opposite side move inward.
- **`DOUBLE` (2):** items on the same side *and* the opposite side as the cursor move outward, items perpendicular to it move inward.

### Items

Each item in `item_vector` (and in any `submenu_array`) has the following fields:

| Key | Type | Description |
|-----|------|-------------|
| `title` | string | Display name shown on hover |
| `type` | string | `"button"` to launch a command, `"sub-menu"` to open a nested ring |
| `icon_path` | string | Path to the item's icon (PNG or other raylib-supported format, or SVG) |
| `icon_size` | int | Icon size in pixels (default: 60) |
| `launch_command` | string | Shell command to run (buttons only) |
| `rotation` | float | Rotation offset in radians applied when this submenu is opened |
| `submenu_array` | array | Nested items (for `"sub-menu"` type) |

### Icons

- Regular image formats are loaded with raylib.
- SVG files are rasterised at 128px wide using nanosvg.
- If the icon path doesn't exist, a built-in default icon is used instead: a folder icon for submenus, an app icon for buttons.

---


## Project Structure

```
ray-pie/
├── main.cpp             # Entry point, window setup, main render loop
├── backend.h            # ITEM, RadialMenu, settings structs, popup manager, GUI state declarations
├── backend.cpp          # ITEM methods, radial menu logic, SVG loading, key input, popups
├── gui.cpp              # Edit mode: item editor + settings editor GUI and their button handlers
├── file_managing.cpp    # Config file loading, saving and default generation
├── images.h             # Embedded PNG data (connecting line, default app and folder icons)
├── json.hpp             # nlohmann/json (bundled, v3.12.0)
├── nanosvg.h            # SVG parser (header-only)
├── nanosvgrast.h        # SVG rasteriser (header-only)
└── raygui.h             # Immediate-mode GUI used by edit mode
```

---

## Notes

- Commands are launched with `nohup ... >/dev/null 2>&1 &` so they detach from the launcher process
- The normal window is fixed at 800×800 with the menu centred at (400, 400); edit mode uses 1400×800 with the menu centred at (1000, 400)
- Edit mode uses an opaque black background; normal mode uses a transparent one
- The transparency is only available if your compositor supports it, else it will render as a black background
- The program does not automatically open with a keybind, that if for YOU to setup using your own DE or WM.
- This application is linux only tho with some small tweaks it could work on windows as well I guess...
