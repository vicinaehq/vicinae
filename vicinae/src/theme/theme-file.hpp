#pragma once
#include "expected.hpp"
#include <filesystem>
#include <qcolor.h>
#include <qjsondocument.h>
#include <unordered_map>
#include "theme/colors.hpp"
#include "lib/toml.hpp"

enum class ThemeTint {
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

enum class ThemeVariant { Light, Dark };

class ThemeFile {
public:
  using Tints = std::unordered_map<ThemeTint, QColor>;
  using Semantics = std::unordered_map<SemanticColor, QColor>;
  using Icon = std::optional<std::filesystem::path>;
  struct InitData {
    QString id;
    QString name;
    QString description;
    std::optional<std::filesystem::path> icon;
    Tints tints;
    ThemeVariant variant;
    Semantics semantics;
  };

  static tl::expected<ThemeFile, QString> fromFile(const std::filesystem::path &path);
  explicit ThemeFile(const InitData &data) : m_data(data) {}
  ThemeFile(const ThemeFile &file) = default;

  const QString &id() const;
  const QString &name() const;
  const QString &description() const;
  const Icon &icon() const;
  ThemeVariant variant() const;
  bool isLight() const;
  bool isDark() const;

  QColor resolve(SemanticColor color) const;
  QColor resolve(ThemeTint tint) const;

  toml::table toToml() const;
  QJsonDocument toJson() const;

  static ThemeFile vicinaeDark();
  static ThemeFile vicinaeLight();

private:
  static ThemeVariant parseVariant(const std::string &variant);
  static std::string serializeVariant(ThemeVariant variant);
  static std::optional<SemanticColor> semanticFromKey(const std::string &key);
  static std::optional<ThemeTint> tintFromKey(const std::string &key);
  static std::optional<std::string> keyFromTint(ThemeTint tint);
  static std::optional<std::string> keyFromSemantic(SemanticColor color);
  static QColor parseColorName(const QString &colorName, const Tints &tints);
  static QColor parseColor(toml::node_view<toml::node> node, const Tints &tints);
  static QColor withAlphaF(const QColor &color, float alpha = 1.0f);

  /**
   * Derive semantic color from base16 palette
   */
  QColor deriveSemantic(SemanticColor color) const;

  InitData m_data;
};
