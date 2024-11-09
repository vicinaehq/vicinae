#include "config.hpp"

std::unique_ptr<Config> loadConfig(std::string_view path) {
  Config cfg;
  toml::table tbl = toml::parse_file(path);

  if (toml::array *arr = tbl["extensions"].as_array()) {
    for (const auto &item : *arr) {
      if (const toml::table *ext = item.as_table()) {
        ConfigExtension extension;

        extension.name = ext->get_as<std::string>("name")->value_or("Unknown");
        extension.enabled = ext->get_as<bool>("enabled")->value_or(false);

        if (const toml::array *arr = ext->get("exec")->as_array()) {
          for (const auto &s : *arr) {
            extension.exec.push_back(s.as_string()->value_or(""));
          }
        }

        cfg.extensions.push_back(extension);
      }
    }
  }

  return std::make_unique<Config>(cfg);
}
