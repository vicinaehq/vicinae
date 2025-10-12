#pragma once
#include "expected.hpp"
#include <filesystem>
#include <qcolor.h>

struct ThemeFileParsingError {};

struct Tints {};

enum class ThemeFileTint {
  Base00,
  Base01,
  Base02,
  Base03,
  Base04,
  Base05,
  Base06,
  Base07,
  Base08,
  Base09,
  Base0A,
  Base0B,
  Base0C,
  Base0D,
  Base0E,
  Base0F,

  Base10,
  Base11,
  Base12,
  Base13,
  Base14,
  Base15,
  Base16,
  Base17
};

class ThemeFile {
public:
  static tl::expected<ThemeFile, QString> fromFile(const std::filesystem::path &path);

  QColor resolveTint(ThemeFileTint tint) const {
    if (auto it = m_tints.find(tint); it != m_tints.end()) { return it->second; }
    return QColor();
  }

private:
  ThemeFile(const std::filesystem::path &path);

  QString m_name;
  QString m_description;
  std::filesystem::path m_icon;
  std::unordered_map<ThemeFileTint, QColor> m_tints;
};
