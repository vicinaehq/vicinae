#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <optional>
#include <string>
#include "../utils/utils.hpp"

namespace xdgpp {

enum class XdgTerminalSelectionState { Selected, FallbackExcluded, FallbackProtected };

struct XdgTerminalEntry {
  std::string_view entryId;
  std::optional<std::string_view> actionId;
};

inline void
parseXdgTerminalsList(const std::filesystem::path &path,
                      const std::function<void(XdgTerminalEntry, XdgTerminalSelectionState)> &cb) {
  std::ifstream ifs{path};
  std::string line{};

  while (std::getline(ifs, line)) {
    auto const entry = trim(line);
    if (entry.empty() || entry.starts_with('#')) continue;
    if (entry.find(".desktop") == std::string_view::npos) continue;

    auto raw = entry;
    XdgTerminalSelectionState state = XdgTerminalSelectionState::Selected;

    if (raw[0] == '+') {
      raw = raw.substr(1);
      state = XdgTerminalSelectionState::FallbackProtected;
    } else if (raw[0] == '-') {
      raw = raw.substr(1);
      state = XdgTerminalSelectionState::FallbackExcluded;
    }

    auto colonPos = raw.find(':');
    if (colonPos != std::string_view::npos) {
      cb({.entryId = raw.substr(0, colonPos), .actionId = raw.substr(colonPos + 1)}, state);
    } else {
      cb({.entryId = raw}, state);
    }
  }
}

}; // namespace xdgpp
