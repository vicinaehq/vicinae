#pragma once
#include "script/script-command-file.hpp"

struct ScriptScanner {
  struct ScannedDirectory {
    std::string id;
    std::filesystem::path path;
    std::uint8_t depth = 0;
  };

  static std::vector<std::shared_ptr<ScriptCommandFile>> scan(std::span<const std::filesystem::path> dirs);
};
