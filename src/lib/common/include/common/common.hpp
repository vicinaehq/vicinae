#pragma once
#include <filesystem>
#include <fstream>
#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace vicinae {
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
