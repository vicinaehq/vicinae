#pragma once
#include <filesystem>

namespace vicinae {
std::filesystem::path selfPath();
std::optional<std::filesystem::path> findHelperProgram(std::string_view program);
}; // namespace vicinae
