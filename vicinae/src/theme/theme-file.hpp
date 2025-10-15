#pragma once
#include "expected.hpp"
#include <filesystem>
#include <qcolor.h>
#include <qjsondocument.h>
#include <unordered_map>
#include "theme/colors.hpp"

enum class ThemeVariant { Light, Dark };

class ThemeFile {
public:
  using Semantics = std::unordered_map<SemanticColor, QColor>;
  using Icon = std::optional<std::filesystem::path>;
  struct InitData {
    QString id;
    QString name;
    QString description;
    QString inherits;
    std::optional<std::filesystem::path> icon;
    ThemeVariant variant;
    Semantics semantics;
  };

  static tl::expected<ThemeFile, QString> fromFile(const std::filesystem::path &path);
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

  QColor resolve(SemanticColor color) const;

  std::string toToml() const;
  QJsonDocument toJson() const;

  static ThemeFile vicinaeDark();
  static ThemeFile vicinaeLight();

private:
  static ThemeVariant parseVariant(const std::string &variant);
  static std::string serializeVariant(ThemeVariant variant);
  static std::optional<SemanticColor> semanticFromKey(const std::string &key);
  static std::optional<std::string> keyFromSemantic(SemanticColor color);
  static QColor withAlphaF(const QColor &color, float alpha = 1.0f);

  /**
   * Derive semantic color from base16 palette
   */
  QColor deriveSemantic(SemanticColor color) const;
  QColor inherit(SemanticColor color) const;

  std::shared_ptr<ThemeFile> m_parent;
  InitData m_data;
};
