#pragma once
#include <cassert>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

inline std::string slurp(const std::filesystem::path &path) {
  const std::ifstream ifs(path);
  std::ostringstream oss;
  oss << ifs.rdbuf();
  return oss.str();
}

inline std::string_view trim(std::string_view str) {
  const auto start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string_view::npos) return {};
  const auto end = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(start, end - start + 1);
}
