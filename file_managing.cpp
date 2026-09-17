
/*
This file contains all the code for interacting with the config files for the pie menu.
The config file is saved as a json and is saved in ~/.config/ray-pie/config.json
*/



#include "backend.h"

std::string get_config_path();
void generate_default_config_file();


// locates and loads the config file from memory
SaveConfig get_config_file() {

  std::string path = get_config_path();
  if (std::filesystem::exists(path) == false) {
    generate_default_config_file();
  }
  if (DEBUG_LOG_RAYPIE) { std::cout << std::endl << "Ignore Texture Unloading Below, its part of the config file loading process" << std::endl; }
  std::ifstream inFile(path);
  json loadedJson;
  inFile >> loadedJson;

  return loadedJson.get<SaveConfig>();
}

// generates a config file that has 1 button, 1 submenu and 1 sub-button
void generate_default_config_file() {
  SaveConfig default_save;
  settings_struct settings;
  std::vector<ITEM> sample_items;

  ITEM a;
  a.title = "Example Button";
  a.type = "button";
  a.icon_path = "path/to/icon";
  a.launch_command = "";
  sample_items.push_back(a);
  ITEM b;
  b.title = "Example Submenu";
  b.type = "sub-menu";
  b.icon_path = "path/to/icon";
  ITEM c;
  c.title = "Example Nested Button";
  c.type = "button";
  c.icon_path = "path/to/icon";
  c.launch_command = "";
  b.submenu_array.push_back(c);
  sample_items.push_back(b);

  default_save.item_vector = sample_items;
  default_save.settings = settings;

  std::string path = get_config_path();
  json j = default_save;
  std::ofstream file(path);
  file << j.dump(4);
  file.close();
  std::cout << "a new config file has been generated successfully" << std::endl;
  
}

void save_config_file(settings_struct *settings, std::vector<ITEM> *item_arr) {
  SaveConfig new_config;
  new_config.item_vector = *item_arr;
  new_config.settings = *settings;

  std::string path = get_config_path();
  json j = new_config;
  std::ofstream file(path);
  file << j.dump(4);
  file.close();
  std::cout << "Config File Saved Successfully" << std::endl;
}

// helper function to get the config path
std::string get_config_path() {
  const char* homedir = std::getenv("HOME");

  if (!homedir) {
    throw std::runtime_error("HOME environment variable not set");
  }
  std::filesystem::create_directory(std::string(homedir) + "/.config/ray-pie");

  return std::string(homedir) + "/.config/ray-pie/config.json";
}