# ray-pie

A lightweight radial launcher for Linux, built with C++ and [raylib](https://www.raylib.com/). Items are arranged in a circle around a central point and launched with a single click — supports nested submenus, smooth animations, and full JSON-based configuration.

---

## Features

- Radial menu with animated item positioning
- Nested submenus (click to dive in, click centre to go back)
- Smooth lerp-based movement and icon scaling on hover
- Transparent window (compositing required)
- Fully configurable via a JSON file (`~/.config/ray-pie/config.json`)
- Auto-generates a default config on first run
- Icon support via texture loading
- Dynamic mouse-proximity item movement (optional)

---

## Dependencies

- [raylib](https://www.raylib.com/) — windowing, rendering, input
- [nlohmann/json](https://github.com/nlohmann/json) (bundled as `json.hpp`) — config parsing
- A C++17-capable compiler (GCC, Clang)
- A compositor supporting transparent windows (e.g. Hyprland, Picom)

---

## Building

Requires `raylib` to be installed on your system.

```bash
# Clone the repo
git clone <repo-url>
cd ray-pie

# Build and run
make
```

This compiles with:

```
g++ main.cpp backend.h backend.cpp file_managing.cpp json.hpp -o build/ray-pie -w -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

The binary is output to `build/ray-pie`. Running `make` will also launch it immediately after a successful build.

---

## Usage

Run the binary to open the launcher:

```bash
./ray-pie
```

- **Hover** over an item to highlight it and see its label
- **Left click** a button item to launch its command
- **Left click** a submenu item to open it
- **Left click** the red centre circle to close a submenu or exit the launcher
- Hold **A / D** to rotate the wheel manually

---

## Configuration

The config file is located at:

```
~/.config/ray-pie/config.json
```

If the file doesn't exist, a default one is generated automatically on first launch.

### Settings

| Key | Type | Description |
|-----|------|-------------|
| `radius` | int | Distance of items from the centre (default: 250) |
| `rotation_offset` | float | Initial rotation of the wheel in radians |
| `animate_speed` | float | Lerp speed for item movement (0.0–1.0) |
| `icon_scale` | float | Global icon scale multiplier |
| `dynamic_movement` | bool | Items shift toward the cursor when nearby |
| `centre_pointer` | bool | Warp cursor to centre on open |
| `centre_icon` | string | Path to an icon displayed at the centre |

### Items

Each item in `item_vector` has the following fields:

| Key | Type | Description |
|-----|------|-------------|
| `title` | string | Display name shown on hover |
| `type` | string | `"button"` to launch a command, `"sub-menu"` to open a nested ring |
| `icon_path` | string | Path to the item's icon image |
| `launch_command` | string | Shell command to run (buttons only) |
| `rotation` | float | Rotation offset applied when this submenu is opened |
| `submenu_array` | array | Nested items (for `"sub-menu"` type) |

### Example

```json
{
  "item_vector": [
    {
      "icon_path": "/path/to/icon",
      "icon_size": 60,
      "launch_command": "",
      "rotation": 0.0,
      "submenu_array": [],
      "title": "Example Button",
      "type": "button"
    },
    {
      "icon_path": "/path/to/icon",
      "icon_size": 60,
      "launch_command": "",
      "rotation": 0.0,
      "submenu_array": [
        {
          "icon_path": "path/to/icon",
          "icon_size": 60,
          "launch_command": "",
          "rotation": 0.0,
          "submenu_array": [],
          "title": "Example Nested Button",
          "type": "button"
        }
      ],
      "title": "Example Submenu",
      "type": "sub-menu"
    }
  ],
  "settings": {
    "animate_speed": 0.20000000298023224,
    "centre_icon": "",
    "centre_pointer": false,
    "dynamic_movement": false,
    "icon_scale": 1.0,
    "radius": 250,
    "rotation_offset": 0.0
  }
}
```

---

## Project Structure

```
ray-pie/
├── main.cpp            # Entry point, render loop, input handling
├── backend.h           # ITEM class, settings structs, helper declarations
├── backend.cpp         # ITEM methods (lerp, render, button press, etc.)
├── file_managing.cpp   # Config file loading and default generation
├── images.h            # Embedded line texture data
└── json.hpp            # nlohmann/json (bundled, v3.12.0)
```

---

## Notes

- Commands are launched with `nohup ... >/dev/null 2>&1 &` so they detach from the launcher process
- Memory cleanup for textures is currently done manually in `cleanup_memory()` rather than via destructors
- The window is fixed at 800×800 with the menu centred at (400, 400)