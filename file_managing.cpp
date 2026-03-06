
/*
This file contains all the code for interacting with the config files for the pie menu.
The config file is saved as a toml and is saved in ~/.config/ray-pie/config.toml
For parsing the toml, the library tomlplusplus is used.
Function overview:
load_toml : responsible for getting the config file and loading it in as a toml::table
toml::tables are the main way that that data is stored.
toml++ lib stores the entire config file into one table. This main table contains settings, buttons and sub-menus
which are all themselves tables.
load_settings : extracts the settings table from the main table and loads it into the settings struct
load_items : takes the main table and loops through every table that isn't settings (these are presumed to be either buttons or sub-menus)
                      depending on the type of table it loaded (found by checking the type variable in the table), either load_submenu or load_button is called.
load_button : takes in a button table and creates a new BUTTON class with it.
load_submenu : takes in a submenu table and creates a new SUB_MENU class with it
*/

/*
The following is info on what types the as() function can return
The template type argument of node::as() must be one of:
|
| A native TOML value type
|   - std::string
|   - int64_t
|   - double
|   - bool
|   - toml::date
|   - toml::time
|   - toml::date_time
|
| A TOML node type
|   - toml::table
|   - toml::array
|   - toml::value<std::string>
|   - toml::value<int64_t>
|   - toml::value<double>
|   - toml::value<bool>
|   - toml::value<toml::date>
|   - toml::value<toml::time>
|   - toml::value<toml::date_time>
*/



#include "backend.h"


void create_toml() {

}

// load the toml config file into a table
int load_toml(toml::table& table) {

  try {
    // read from a file
    const char* homedir = std::getenv("HOME");

    if (!homedir) {
      throw std::runtime_error("HOME environment variable not set");
    }

    std::string path = std::string(homedir) + "/.config/ray-pie/config.toml";

    table = toml::parse_file(path);
    //table = toml::parse_file("/home/jonathan/Dev/C++/ray-pie/config.toml");
  }
  catch (const toml::parse_error& err) {
    std::cerr << "warning there was an error parsing the config file: " << err << "\n";
    return 1;
  }
  return 0;
}

// load the settings of raypie into a struct
int load_settings(settings_struct& settings, toml::table tbl) {

  // Extract the settings section
  auto settings_tbl = tbl["Settings"].as_table();

  if (!settings_tbl) {
    std::cerr << "No [settings] section found!" << std::endl;
    return 1;
  }

  try {
    settings.animate_speed = settings_tbl->get_as<double>("animate_speed")->get();
    settings.radius = settings_tbl->get_as<int64_t>("radius")->get();
    settings.rotation_offset = settings_tbl->get_as<double>("rotation_offset")->get();
    settings.icon_scale = settings_tbl->get_as<double>("icon_scale")->get();
    settings.dynamic_movement = settings_tbl->get_as<bool>("dynamic_movement")->get();
    settings.centre_pointer = settings_tbl->get_as<bool>("centre_pointer")->get();
    settings.centre_icon = settings_tbl->get_as<std::string>("centre_icon")->get().c_str();
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    std::cerr << "a setting is either missing or spelt incorrectly..." << std::endl;
  }

  return 0;
}

void load_items(toml::table tbl, std::vector<ITEM>& item_vector) {
  // loop through main table and extracts items
  std::vector<ITEM> temp_vector;
  int heightest_nest_lvl = 0;
  for (auto const& [key, value] : tbl) {

    if (value.is_table()) {
      std::cout << key << " is a table." << std::endl;

      // skip the settings table
      if (key == "Settings") {
        continue;
      };
      auto item_table = tbl[key].as_table();

      if (item_table->get_as<std::string>("type")->get() == "button") {
        if (item_table->get_as<int64_t>("nest_level")->get() > heightest_nest_lvl) { heightest_nest_lvl = item_table->get_as<int64_t>("nest_level")->get(); }
        temp_vector.push_back(load_button(item_table));

      };
      if (item_table->get_as<std::string>("type")->get() == "sub-menu") {
        temp_vector.push_back(load_submenu(item_table));
      };
    }
  }

  for (; heightest_nest_lvl > -1; heightest_nest_lvl--) {
    for (auto& i : temp_vector) {
      if (i.nest_level == heightest_nest_lvl) {
        for (auto& j : temp_vector) {
          if (j.title == i.parent) {
            j.submenu_array.push_back(i);
            std::cout << j.title << "  :  " << i.title << std::endl;
            break;
          }
        }

      }
    }
  }
  for (auto i : temp_vector) {
    if (i.parent == "root") {
      item_vector.push_back(i);
    }
  }
}


// takes in a table and creates a BUTTON
ITEM load_button(toml::v3::table* tbl) {
  ITEM b;

  b.nest_level = tbl->get_as<int64_t>("nest_level")->get();
  b.title = tbl->get_as<std::string>("title")->get();
  b.parent = tbl->get_as<std::string>("parent")->get();
  b.type = tbl->get_as<std::string>("type")->get();
  b.launch_command = tbl->get_as<std::string>("command")->get();
  b.icon_texture = LoadTexture(tbl->get_as<std::string>("icon_path")->get().c_str());

  return b;
}

// takes in a table and creates a SUB-MENU
ITEM load_submenu(toml::v3::table* tbl) {
  ITEM s;
  s.title = tbl->get_as<std::string>("title")->get();
  s.parent = tbl->get_as<std::string>("parent")->get();
  s.type = tbl->get_as<std::string>("type")->get();
  s.rotation = tbl->get_as<double>("rotation")->get();
  s.icon_texture = LoadTexture(tbl->get_as<std::string>("icon_path")->get().c_str());

  return s;
}

