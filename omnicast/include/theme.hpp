#pragma once
#include "omnicast.hpp"
#include <filesystem>
#include <libqalculate/includes.h>
#include <qapplication.h>
#include <qcolor.h>
#include <qfilesystemwatcher.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
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

struct ColorPalette {
  QColor background;
  QColor foreground;
  QColor blue;
  QColor green;
  QColor magenta;
  QColor orange;
  QColor purple;
  QColor red;
  QColor yellow;
  QColor cyan;
};

struct ParsedThemeData {
  QString id;
  QString appearance;
  QString name;
  QString description;
  std::optional<std::filesystem::path> icon;
  ColorPalette palette;
};

struct ThemeInfo {
  QString appearance;
  QString id;
  QString name;
  QString description;
  std::optional<std::filesystem::path> icon;
  std::optional<std::filesystem::path> path;

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

    info.id = scheme.name;
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

  static ThemeInfo fromParsed(const ParsedThemeData &scheme) {
    ThemeInfo info;

    info.id = scheme.id;
    info.name = scheme.name;
    info.appearance = scheme.appearance;
    info.icon = scheme.icon;
    info.description = scheme.description;
    info.colors.blue = scheme.palette.blue;
    info.colors.green = scheme.palette.green;
    info.colors.magenta = scheme.palette.magenta;
    info.colors.orange = scheme.palette.orange;
    info.colors.purple = scheme.palette.purple;
    info.colors.red = scheme.palette.red;
    info.colors.yellow = scheme.palette.yellow;
    info.colors.mainBackground = scheme.palette.background;

    if (scheme.appearance == "dark") {
      info.colors.border = info.colors.mainBackground.lighter(180);
      info.colors.mainSelectedBackground = info.colors.mainBackground.lighter(135);
      info.colors.mainHoveredBackground = info.colors.mainBackground.lighter(140);
      info.colors.statusBackground = info.colors.mainBackground.lighter(140);
      info.colors.statusBackgroundLighter = info.colors.statusBackground.lighter(150);
      info.colors.statusBackgroundHover = info.colors.statusBackground.lighter(120);
      info.colors.statusBackgroundBorder = info.colors.statusBackground.lighter(180);
      info.colors.text = scheme.palette.foreground;
      info.colors.subtext = scheme.palette.foreground.darker(150);
    } else {
      info.colors.border = info.colors.mainBackground.darker(130);
      info.colors.mainSelectedBackground = info.colors.mainBackground.darker(110);
      info.colors.mainHoveredBackground = info.colors.mainBackground.darker(115);
      info.colors.statusBackground = info.colors.mainBackground.darker(110);
      info.colors.statusBackgroundLighter = info.colors.statusBackground.darker(130);
      info.colors.statusBackgroundHover = info.colors.statusBackground.darker(100);
      info.colors.statusBackgroundBorder = info.colors.statusBackground.darker(160);
      info.colors.text = scheme.palette.foreground;
      info.colors.subtext = scheme.palette.foreground.lighter(130);
    }

    return info;
  }
};

class ThemeService : public QObject {
  Q_OBJECT

  std::vector<ThemeInfo> m_themes;
  ThemeInfo m_theme;
  int m_baseFontPointSize = 10;
  std::filesystem::path m_configDir = Omnicast::configDir();
  std::filesystem::path m_userThemeDir = m_configDir / "themes";
  std::filesystem::path m_dataThemeDir = Omnicast::dataDir() / "themes";

  ThemeService(const ThemeService &rhs) = delete;
  ThemeService &operator=(const ThemeService &rhs) = delete;

  ThemeService() {
    registerBuiltinThemes();
    scanThemeDirectories();
  }

public:
  static ThemeService &instance() {
    static ThemeService _instance;

    return _instance;
  }

  int pointSize(TextSize size) const {
    switch (size) {
    case TextSize::TextRegular:
      return m_baseFontPointSize;
    case TextSize::TextTitle:
      return m_baseFontPointSize * 1.5;
    }

    return m_baseFontPointSize;
  }

  /**
   * Returns the theme that is currently in use
   */
  const ThemeInfo &theme() const { return m_theme; }

  std::vector<ParsedThemeData> loadColorSchemes() const;

  std::optional<ThemeInfo> theme(const QString &name) const {
    for (const auto &info : m_themes) {
      if (info.name == name) { return info; }
    }

    return std::nullopt;
  }

  bool setTheme(const QString &name) {
    for (const auto &info : m_themes) {
      if (info.id == name) {
        setTheme(info);
        return true;
      }
    }

    return false;
  }

  ColorLike getTintColor(ColorTint tint) const {
    switch (tint) {
    case ColorTint::Blue:
      return m_theme.colors.blue;
    case ColorTint::Green:
      return m_theme.colors.green;
    case ColorTint::Magenta:
      return m_theme.colors.magenta;
    case ColorTint::Orange:
      return m_theme.colors.orange;
    case ColorTint::Purple:
      return m_theme.colors.purple;
    case ColorTint::Red:
      return m_theme.colors.red;
    case ColorTint::Yellow:
      return m_theme.colors.yellow;
    case ColorTint::TextPrimary:
      return m_theme.colors.text;
    case ColorTint::TextSecondary:
      return m_theme.colors.subtext;
    default:
      break;
    }

    return {};
  }

  void setTheme(const ThemeInfo &info);

  void registerTheme(const ThemeInfo &info) { m_themes.emplace_back(info); }
  const std::vector<ThemeInfo> &themes() const { return m_themes; }

  void registerBuiltinThemes();
  std::optional<ThemeInfo> findTheme(const QString &name);
  void upsertTheme(const ParsedThemeData &data);
  void scanThemeDirectory(const std::filesystem::path &path);
  void handleDirectoryChanged(const QString &directory);
  void scanThemeDirectories();

signals:
  bool themeChanged(const ThemeInfo &info) const;
};
