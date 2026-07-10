#pragma once
#include <filesystem>
#include <fstream>
#include <istream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace vicinae {
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
