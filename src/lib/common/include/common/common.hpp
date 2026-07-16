#pragma once
#include <array>
#include <filesystem>
#include <fstream>
#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace vicinae {
inline constexpr std::array<std::string_view, 3> APP_SCHEMES{"vicinae", "raycast", "com.raycast"};

// true for app deeplinks such as vicinae://toggle or com.raycast:/extensions/...
bool isAppDeeplink(std::string_view url);

// Switches the CRT locale to UTF-8 on Windows so that we operate on UTF-8 rather
// than the legacy ANSI code page. No-op elsewhere.
void enableUtf8();

std::filesystem::path selfPath();
std::optional<std::filesystem::path> findHelperProgram(std::string_view program);
std::vector<std::filesystem::path> helperProgramCandidates(std::string_view program);
std::string slurp(std::istream &ifs);

std::optional<std::filesystem::path> findServerBinary();

std::filesystem::path runtimeDir();
std::filesystem::path serverSocketPath();

std::string currentUserName();
std::string serverSocketName();
}; // namespace vicinae
