#pragma once
#include <filesystem>
#include <vector>
#include <variant>
#include <qapplication.h>
#include <qcolor.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qobject.h>
#include <QWidget>

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
  Cyan,

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
  // Base16 colors
  QColor base00;
  QColor base01;
  QColor base02;
  QColor base03;
  QColor base04;
  QColor base05;
  QColor base06;
  QColor base07;
  QColor base08;
  QColor base09;
  QColor base0A;
  QColor base0B;
  QColor base0C;
  QColor base0D;
  QColor base0E;
  QColor base0F;
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

  struct Colors {
    // Base16 colors
    QColor base00;
    QColor base01;
    QColor base02;
    QColor base03;
    QColor base04;
    QColor base05;
    QColor base06;
    QColor base07;
    QColor base08;
    QColor base09;
    QColor base0A;
    QColor base0B;
    QColor base0C;
    QColor base0D;
    QColor base0E;
    QColor base0F;
  } colors;

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

private:
  static void extractColorPalette(ColorPalette &palette, const QJsonObject &colors);
  static bool validateThemeData(ParsedThemeData &theme, const QJsonObject &obj,
                                const std::filesystem::path &entry, const std::filesystem::path &dir);

signals:
  bool themeChanged(const ThemeInfo &info) const;
};
