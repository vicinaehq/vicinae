#pragma once
#include "file.hpp"
#include <vector>
#include <filesystem>

namespace xdgpp {
std::vector<DesktopFile> getAllDesktopEntries();
std::vector<DesktopFile> getAllDesktopEntries(const std::vector<std::filesystem::path> &paths);
}; // namespace xdgpp
