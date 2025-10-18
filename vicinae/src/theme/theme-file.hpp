#pragma once
#include "expected.hpp"
#include <algorithm>
#include <filesystem>
#include <qcolor.h>
#include <qjsondocument.h>
#include <unordered_map>
#include "theme/colors.hpp"

enum class ThemeVariant { Light, Dark };

class ThemeFile {
public:
  struct ColorRef {
    SemanticColor color;
    std::optional<double> opacity;
    std::optional<int> lighter;
    std::optional<int> darker;
  };

  using MappedColor = std::variant<QColor, ColorRef>;
  using Semantics = std::unordered_map<SemanticColor, MappedColor>;
  using Icon = std::optional<std::filesystem::path>;
  struct InitData {
    QString id;
    QString name;
    QString description;
    QString inherits;
    std::optional<std::filesystem::path> icon;
    std::optional<std::filesystem::path> path;
    ThemeVariant variant;
    Semantics semantics;
  };

  static tl::expected<ThemeFile, std::string> fromFile(const std::filesystem::path &path);
  explicit ThemeFile(const InitData &data) : m_data(data) {}
  ThemeFile(const ThemeFile &file) = default;

  void setParent(const std::shared_ptr<ThemeFile> &file);
  const QString &id() const;
  const QString &name() const;
  const QString &description() const;
  const Icon &icon() const;
  ThemeVariant variant() const;
  /**
   * A theme always inherits either one of the two base themes (vicinae-dark/vicinae-light)
   * or any other theme that is loaded.
   */
  const QString &inherits() const;
  bool isLight() const;
  bool isDark() const;

  /**
   * The path to the theme file, if the theme was loaded from an actual file.
   */
  std::optional<std::filesystem::path> path() const;

  QColor resolve(SemanticColor color) const;

  /**
   * Resolve the color and convert it to a string representation
   * suitable for use in CSS stylesheets or general display.
   * This defaults to rgba(R, G, B, A)
   */
  QString resolveAsString(SemanticColor color) const;

  std::string toToml() const;

  static ThemeFile vicinaeDark();
  static ThemeFile vicinaeLight();

  static std::optional<SemanticColor> semanticFromKey(const std::string &key);
  static std::optional<std::string> keyFromSemantic(SemanticColor color);

private:
  static QColor withAlphaF(const QColor &color, float alpha = 1.0f);
  // QColor recursiveResolve(SemanticColor color) const;

  /**
   * Derive semantic color from base16 palette
   */
  QColor deriveSemantic(SemanticColor color) const;
  QColor inherit(SemanticColor color) const;

  std::shared_ptr<ThemeFile> m_parent;
  InitData m_data;
};
