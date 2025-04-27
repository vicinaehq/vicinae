#pragma once
#include <qapplication.h>
#include <qcolor.h>
#include <qobject.h>
#include <QWidget>
#include <qpalette.h>
#include <qtmetamacros.h>

enum ColorTint { InvalidTint, Blue, Green, Magenta, Orange, Purple, Red, Yellow, TextPrimary, TextSecondary };
enum TextSize { TextRegular, TextTitle };

struct ThemeLinearGradient {
  std::vector<QColor> points;
};

struct ThemeRadialGradient {
  std::vector<QColor> points;
};

using ColorLike = std::variant<QColor, ThemeLinearGradient, ThemeRadialGradient, ColorTint>;

using DeclarativeColor = std::variant<ColorTint, QColor>;

struct ColorScheme {
  QString name;
  QColor background;
  QColor foreground;
  QColor blue;
  QColor green;
  QColor magenta;
  QColor orange;
  QColor purple;
  QColor red;
  QColor yellow;
};

struct ThemeInfo {
  QString name;

  struct {
    QColor text;
    QColor subtext;
    QColor border;

    QColor mainBackground;
    QColor mainSelectedBackground;
    QColor mainHoveredBackground;

    QColor statusBackground;
    QColor statusBackgroundBorder;
    QColor statusBackgroundHover;
    QColor statusBackgroundLighter;

    ColorLike blue;
    ColorLike green;
    ColorLike magenta;
    ColorLike orange;
    ColorLike purple;
    ColorLike red;
    ColorLike yellow;
  } colors;

  static ThemeInfo fromColorScheme(const ColorScheme &scheme) {
    ThemeInfo info;

    info.name = scheme.name;
    info.colors.blue = scheme.blue;
    info.colors.green = scheme.green;
    info.colors.magenta = scheme.magenta;
    info.colors.orange = scheme.orange;
    info.colors.purple = scheme.purple;
    info.colors.red = scheme.red;
    info.colors.yellow = scheme.yellow;
    info.colors.mainBackground = scheme.background;
    info.colors.border = info.colors.mainBackground.lighter(180);
    info.colors.mainSelectedBackground = info.colors.mainBackground.lighter(135);
    info.colors.mainHoveredBackground = info.colors.mainBackground.lighter(140);
    info.colors.statusBackground = info.colors.mainBackground.lighter(140);
    info.colors.statusBackgroundLighter = info.colors.statusBackground.lighter(150);
    info.colors.statusBackgroundHover = info.colors.statusBackground.lighter(120);
    info.colors.statusBackgroundBorder = info.colors.statusBackground.lighter(180);
    info.colors.text = scheme.foreground;
    info.colors.subtext = scheme.foreground.darker(150);

    return info;
  }
};

class ThemeService : public QObject {
  Q_OBJECT

  std::vector<ThemeInfo> _themeDb;
  ThemeInfo _theme;
  int _baseFontPointSize = 10;

  ThemeService(const ThemeService &rhs) = delete;

public:
  static ThemeService &instance() {
    static ThemeService _instance;

    return _instance;
  }

  int pointSize(TextSize size) const {
    switch (size) {
    case TextSize::TextRegular:
      return _baseFontPointSize;
    case TextSize::TextTitle:
      return _baseFontPointSize * 1.5;
    }

    return _baseFontPointSize;
  }

  /**
   * Returns the theme that is currently in use
   */
  const ThemeInfo &theme() const { return _theme; }

  std::vector<ColorScheme> loadColorSchemes() const;

  std::optional<ThemeInfo> theme(const QString &name) const {
    for (const auto &info : _themeDb) {
      if (info.name == name) { return info; }
    }

    return std::nullopt;
  }

  bool setTheme(const QString &name) {
    for (const auto &info : _themeDb) {
      if (info.name == name) {
        setTheme(info);
        return true;
      }
    }

    return false;
  }

  ColorLike getTintColor(ColorTint tint) const {
    switch (tint) {
    case ColorTint::Blue:
      return _theme.colors.blue;
    case ColorTint::Green:
      return _theme.colors.green;
    case ColorTint::Magenta:
      return _theme.colors.magenta;
    case ColorTint::Orange:
      return _theme.colors.orange;
    case ColorTint::Purple:
      return _theme.colors.purple;
    case ColorTint::Red:
      return _theme.colors.red;
    case ColorTint::Yellow:
      return _theme.colors.yellow;
    case ColorTint::TextPrimary:
      return _theme.colors.text;
    case ColorTint::TextSecondary:
      return _theme.colors.subtext;
    default:
      break;
    }

    return {};
  }

  void setTheme(const ThemeInfo &info);

  void registerTheme(const ThemeInfo &info) { _themeDb.emplace_back(info); }

  const std::vector<ThemeInfo> &themes() const { return _themeDb; }

  void registerBuiltinThemes() {
    for (const auto &scheme : loadColorSchemes()) {
      registerTheme(ThemeInfo::fromColorScheme(scheme));
    }
  }

  ThemeService() { registerBuiltinThemes(); }

signals:
  bool themeChanged(const ThemeInfo &info) const;
};
