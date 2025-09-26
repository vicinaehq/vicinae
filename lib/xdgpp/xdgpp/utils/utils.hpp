#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

inline std::string slurp(const std::filesystem::path &path) {
  std::ifstream ifs(path);
  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}
