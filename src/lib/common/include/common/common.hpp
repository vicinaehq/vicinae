#pragma once
#include <filesystem>
#include <vector>

namespace vicinae {
std::filesystem::path selfPath();
std::optional<std::filesystem::path> findHelperProgram(std::string_view program);
std::vector<std::filesystem::path> helperProgramCandidates(std::string_view program);
}; // namespace vicinae
