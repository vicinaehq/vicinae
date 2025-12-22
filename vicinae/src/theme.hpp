#pragma once
#include "theme/colors.hpp"
#include <qobject.h>

class ThemeFile;
class ThemeDatabase;

enum TextSize { TextRegular, TextTitle, TextSmaller };

struct ThemeLinearGradient {
  std::vector<QColor> points;
};

struct ThemeRadialGradient {
  std::vector<QColor> points;
};

struct DynamicColor {
  QString light;
  QString dark;
  bool adjustContrast = false;
};

using ColorLike = std::variant<QColor, QString, SemanticColor, DynamicColor>;

class ThemeService : public QObject {
  Q_OBJECT

signals:
  bool themeChanged(const ThemeFile &info) const;

public:
  static ThemeService &instance();

  /**
   * Generate a stylesheet for use by most vicinae inputs.
   * We generally need a stylesheet to style theme because
   * it's the most reliable way.
   */
  QString inputStyleSheet();
  QString nativeFilePickerStyleSheet();

  ThemeService();

  double pointSize(TextSize size) const;

  /**
   * Returns the theme that is currently in use
   */
  const ThemeFile &theme() const;
  ThemeDatabase &db();

  bool setTheme(const QString &name);
  void setTheme(const ThemeFile &info);

  const ThemeFile *findTheme(const QString &id);

  ColorLike getTintColor(SemanticColor tint) const;

  void setFontBasePointSize(double pointSize);

  void reloadCurrentTheme();
  std::vector<std::shared_ptr<ThemeFile>> themes() const;

private:
  std::shared_ptr<ThemeDatabase> m_themeDb;
  std::unique_ptr<ThemeFile> m_theme;

  void applyBaseStyle();

  double m_baseFontPointSize = 10;

  ThemeService(const ThemeService &rhs) = delete;
  ThemeService &operator=(const ThemeService &rhs) = delete;
};
