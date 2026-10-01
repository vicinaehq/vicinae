#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include "../utils/utils.hpp"
#include "xdgpp/env/env.hpp"

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

inline bool setDefaultTerminal(std::string_view appId, std::optional<std::string_view> actionId,
                               const std::filesystem::path &path) {
  constexpr auto HEADER = "# Configured by the Vicinae launcher";
  std::string buf{};
  bool inserted = false;

  const auto insertAppId = [&]() {
    inserted = true;
    buf += appId;

    if (actionId) {
      buf += ':';
      buf += actionId.value();
    }

    buf += "\n";
  };

  const auto insertHeader = [&]() {
    if (!buf.empty()) buf += "\n";
    buf += HEADER;
    buf += "\n";
  };

  {
    std::ifstream ifs{path};
    std::string line{};
    bool writeNext = false;

    while (std::getline(ifs, line)) {
      auto const entry = trim(line);

      if (writeNext && !entry.empty() && !entry.starts_with("#")) {
        writeNext = false;
        insertAppId();
        continue;
      }

      if (!inserted && !entry.empty() && (!entry.starts_with("#") || entry == HEADER)) {
        if (entry == HEADER) {
          writeNext = true; // replace the next non empty line with the new app id
        } else {
          insertHeader();
          insertAppId();
        }
      }

      buf += line + "\n";
    }
  }

  if (!inserted) {
    insertHeader();
    insertAppId();
  }

  std::ofstream ofs{path};
  ofs << buf;

  return ofs.good();
}

inline bool setDefaultTerminal(std::string_view appId, std::optional<std::string_view> actionId) {
  return setDefaultTerminal(appId, actionId, xdgpp::configHome() / "xdg-terminals.list");
}

}; // namespace xdgpp
