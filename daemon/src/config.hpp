#pragma once
#include <string>
#include <toml++/toml.h>
#include <vector>

struct ConfigExtension {
  std::string name;
  bool enabled;
  std::vector<std::string> exec;
};

struct Config {
  std::vector<ConfigExtension> extensions;
};

std::unique_ptr<Config> loadConfig(std::string_view path);
