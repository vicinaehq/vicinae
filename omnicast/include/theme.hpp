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

enum SemanticColor {
  InvalidTint,

  // Basic color palette
  Blue,
  Green,
  Magenta,
  Orange,
  Purple,
  Red,
  Yellow,

  // Text colors
  TextPrimary,
  TextSecondary,
  TextTertiary, // Even more subtle than secondary
  TextDisabled, // For disabled text
  TextOnAccent, // White/black text on colored backgrounds
  TextError,    // Error messages
  TextSuccess,  // Success messages
  TextWarning,  // Warning messages

  // Background colors
  MainBackground,
  MainHoverBackground,
  MainSelectedBackground,
  SecondaryBackground, // Cards, panels
  TertiaryBackground,  // Deep inset areas

  // Button states
  ButtonPrimary,
  ButtonPrimaryHover,
  ButtonPrimaryPressed,
  ButtonPrimaryDisabled,

  ButtonSecondary,
  ButtonSecondaryHover,
  ButtonSecondaryPressed,
  ButtonSecondaryDisabled,

  ButtonDestructive, // Delete, remove actions
  ButtonDestructiveHover,
  ButtonDestructivePressed,

  // Input/form states
  InputBackground,
  InputBorder,
  InputBorderFocus,
  InputBorderError,
  InputPlaceholder,

  // UI elements
  Border,
  BorderSubtle, // Very light borders
  BorderStrong, // Emphasized borders

  Separator, // Divider lines
  Shadow,    // Drop shadows

  // Status/feedback colors
  StatusBackground,
  StatusBorder,
  StatusHover,

  ErrorBackground, // Error state backgrounds
  ErrorBorder,
  SuccessBackground, // Success state backgrounds
  SuccessBorder,
  WarningBackground, // Warning state backgrounds
  WarningBorder,

  // Interactive elements
  LinkDefault,
  LinkHover,
  LinkVisited,

  // Special states
  Focus,       // Focus rings
  Overlay,     // Modal overlays, tooltips
  Tooltip,     // Tooltip backgrounds
  TooltipText, // Tooltip text
};

enum TextSize { TextRegular, TextTitle, TextSmaller };

struct ThemeLinearGradient {
  std::vector<QColor> points;
};

struct ThemeRadialGradient {
  std::vector<QColor> points;
};

using ColorLike = std::variant<QColor, ThemeLinearGradient, ThemeRadialGradient, SemanticColor>;

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
  int m_baseFontPointSize = 10;
  std::filesystem::path m_configDir = Omnicast::configDir();
  std::filesystem::path m_userThemeDir = m_configDir / "themes";
  std::filesystem::path m_dataThemeDir = Omnicast::dataDir() / "themes";

  ThemeService(const ThemeService &rhs) = delete;
  ThemeService &operator=(const ThemeService &rhs) = delete;

  ThemeService();

public:
  static ThemeService &instance();

  int pointSize(TextSize size) const;

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

  void registerBuiltinThemes();
  std::optional<ThemeInfo> findTheme(const QString &name);
  void upsertTheme(const ParsedThemeData &data);
  void scanThemeDirectory(const std::filesystem::path &path);
  void handleDirectoryChanged(const QString &directory);
  void scanThemeDirectories();
  void setDefaultTheme();

signals:
  bool themeChanged(const ThemeInfo &info) const;
};
