#pragma once
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
#include <filesystem>

enum SemanticColor {
  InvalidTint,

  // Basic color palette
  Blue,
  Green,
  Magenta,
  Orange,
  Red,
  Yellow,
  Cyan,
  Purple, // for now, aliased to magenta

  // Background colors
  Background,
  BackgroundBorder,
  LighterBackground,
  LighterBackgroundBorder,
  SelectionBackground,
  HoverBackground,
  LighterSelectionBackground,
  LighterHoverBackground,

  // foreground
  Foreground,
  LightForeground,
  DarkForeground,
  LightestForeground,
  AccentForeground,

  // text selection
  TextSelectionBackground,
  TextSelectionForeground,

  // input
  InputBorder,
  InputBorderFocus,
  InputBorderError,
  InputPlaceholder,
};

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

    QColor blue;
    QColor green;
    QColor magenta;
    QColor orange;
    QColor purple;
    QColor red;
    QColor yellow;
    QColor cyan;

    QColor textTertiary;
    QColor textDisabled;
    QColor textOnAccent;

    QColor secondaryBackground;
    QColor tertiaryBackground;

    QColor buttonPrimary;
    QColor buttonPrimaryHover;
    QColor buttonPrimaryPressed;
    QColor buttonPrimaryDisabled;

    QColor buttonSecondary;
    QColor buttonSecondaryHover;
    QColor buttonSecondaryPressed;
    QColor buttonSecondaryDisabled;

    QColor buttonDestructive;
    QColor buttonDestructiveHover;
    QColor buttonDestructivePressed;

    QColor inputBackground;
    QColor inputBorder;
    QColor inputBorderFocus;
    QColor inputBorderError;
    QColor inputPlaceholder;

    QColor borderSubtle;
    QColor borderStrong;
    QColor separator;
    QColor shadow;

    QColor errorBackground;
    QColor errorBorder;
    QColor successBackground;
    QColor successBorder;
    QColor warningBackground;
    QColor warningBorder;

    QColor linkDefault;
    QColor linkHover;
    QColor linkVisited;

    QColor focus;
    QColor overlay;
    QColor tooltip;
    QColor tooltipText;
  } colors;

  static QColor adjustColorHSL(const QColor &base, int hueShift = 0, float satMult = 1.0f,
                               float lightMult = 1.0f);

  QColor resolveTint(SemanticColor tint) const;

  static ThemeInfo fromParsed(const ParsedThemeData &scheme);
};

class ThemeService : public QObject {
  Q_OBJECT

  std::vector<ThemeInfo> m_themes;
  ThemeInfo m_theme;
  double m_baseFontPointSize = 10;

  ThemeService(const ThemeService &rhs) = delete;
  ThemeService &operator=(const ThemeService &rhs) = delete;

  ThemeService();

public:
  static ThemeService &instance();

  double pointSize(TextSize size) const;

  /**
   * Returns the theme that is currently in use
   */
  const ThemeInfo &theme() const;

  std::vector<ParsedThemeData> loadColorSchemes() const;

  std::optional<ThemeInfo> theme(const QString &name) const;

  bool setTheme(const QString &name);

  ColorLike getTintColor(SemanticColor tint) const;

  void setTheme(const ThemeInfo &info);

  void registerTheme(const ThemeInfo &info) { m_themes.emplace_back(info); }
  const std::vector<ThemeInfo> &themes() const { return m_themes; }

  void setFontBasePointSize(double pointSize);

  void reloadCurrentTheme();

  void registerBuiltinThemes();
  std::optional<ThemeInfo> findTheme(const QString &name);
  void upsertTheme(const ParsedThemeData &data);
  void scanThemeDirectory(const std::filesystem::path &path);
  void handleDirectoryChanged(const QString &directory);

  /**
   * Scan themes in local config directory first, then try to load themes from data directories.
   */
  void scanThemeDirectories();

signals:
  bool themeChanged(const ThemeInfo &info) const;
};
