#pragma once
#include "expected.hpp"
#include <filesystem>
#include <qcolor.h>

struct ThemeFileParsingError {};

struct Tints {};

class ThemeFile {
public:
  static tl::expected<ThemeFile, QString> fromFile(const std::filesystem::path &path);

private:
  ThemeFile(const std::filesystem::path &path);

  Tints m_tints;
};
