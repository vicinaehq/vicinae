#pragma once
#include "expected.hpp"
#include <filesystem>
#include <qcolor.h>
#include <unordered_map>
#include "lib/toml.hpp"
#include "theme.hpp"

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
  using Tints = std::unordered_map<ThemeFileTint, QColor>;
  using Semantics = std::unordered_map<SemanticColor, QColor>;
  struct InitData {
    QString name;
    QString description;
    std::optional<std::filesystem::path> icon;
    Tints tints;
    Semantics semantics;
  };

  QColor resolveSemantic(SemanticColor color) {
    if (auto it = m_data.semantics.find(color); it != m_data.semantics.end()) { return it->second; }
    return deriveSemantic(color);
  }

  QColor resolveTint(ThemeFileTint tint) const {
    if (auto it = m_data.tints.find(tint); it != m_data.tints.end()) { return it->second; }
    return QColor();
  }

  static QColor parseColorName(const QString &colorName, const Tints &tints);
  static QColor parseColor(toml::node_view<toml::node> node, const Tints &tints);

  explicit ThemeFile(const InitData &data) : m_data(data) {}

private:
  /**
   * Derive semantic color from base16 palette
   */
  QColor deriveSemantic(SemanticColor color);

  InitData m_data;
};
