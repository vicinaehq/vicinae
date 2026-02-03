#pragma once
#include <expected>
#include "theme/theme-file.hpp"
#include <algorithm>

class ThemeParser {
public:
  using DiagnosticList = std::vector<std::string>;

  std::expected<ThemeFile, std::string> parse(const std::filesystem::path &path);
  std::string serializeToToml(const ThemeFile &file);
  DiagnosticList diagnostics() const;

private:
  DiagnosticList m_diagnostics;
};

class ThemeSerializer {
public:
  std::string toToml(const ThemeFile &file) const;
};
