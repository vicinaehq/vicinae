#pragma once
#include <filesystem>
#include <optional>
#include <vector>

namespace xdgpp {

/**
 * Current desktop values as provided by the optional XDG_CURRENT_DESKTOP environment
 * variable.
 */
std::vector<std::string> currentDesktop();

std::filesystem::path dataHome();

std::filesystem::path stateHome();

std::filesystem::path cacheHome();

/**
 * XDG data directories.
 * Parsed from the XDG_DATA_DIRS environment variable.
 * If not set, we provide a default set of directories where data is commonly found.
 */
std::vector<std::filesystem::path> dataDirs();

/**
 * Like `dataDirs` but makes sure common directories are included if they are not in the XDG_DATA_DIRS.
 */
std::vector<std::filesystem::path> commonDataDirs();

/**
 * Directories where applications (desktop entries) are looked for.
 * This is essentially dataDirs() but with /applications appended to each path.
 */
std::vector<std::filesystem::path> appDirs();

/**
 * From XDG_CONFIG_HOME, or defaults to $HOME/.config
 */
std::filesystem::path configHome();

/**
 * Parsed from XDG_CONFIG_DIRS.
 * If not set, defaults to `/etc/xdg`.
 */
std::vector<std::filesystem::path> configDirs();

std::optional<std::filesystem::path> runtimeDir();

/**
 * List of possible locations for mimeapps.list files.
 */
std::vector<std::filesystem::path> mimeAppsListPaths();
}; // namespace xdgpp
