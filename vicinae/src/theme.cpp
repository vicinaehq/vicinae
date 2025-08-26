#include "theme.hpp"
#include "timer.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "vicinae.hpp"
#include <QLinearGradient>
#include <filesystem>
#include <QStyleHints>
#include <system_error>

namespace fs = std::filesystem;

QColor ThemeInfo::resolveTint(SemanticColor tint) const {
  switch (tint) {
  // Basic palette
  case SemanticColor::Blue:
    return colors.blue;
  case SemanticColor::Green:
    return colors.green;
  case SemanticColor::Magenta:
    return colors.magenta;
  case SemanticColor::Orange:
    return colors.orange;
  case SemanticColor::Purple:
    return colors.purple;
  case SemanticColor::Red:
    return colors.red;
  case SemanticColor::Yellow:
    return colors.yellow;
  case SemanticColor::Cyan:
    return colors.cyan;

  // Text colors
  case SemanticColor::TextPrimary:
    return colors.text;
  case SemanticColor::TextSecondary:
    return colors.subtext;
  case SemanticColor::TextTertiary:
    return colors.textTertiary;
  case SemanticColor::TextDisabled:
    return colors.textDisabled;
  case SemanticColor::TextOnAccent:
    return colors.textOnAccent;
  case SemanticColor::TextError:
    return colors.red;
  case SemanticColor::TextSuccess:
    return colors.green;
  case SemanticColor::TextWarning:
    return colors.orange;

  // Backgrounds
  case SemanticColor::MainBackground:
    return colors.mainBackground;
  case SemanticColor::MainHoverBackground:
    return colors.mainHoveredBackground;
  case SemanticColor::MainSelectedBackground:
    return colors.mainSelectedBackground;
  case SemanticColor::SecondaryBackground:
    return colors.secondaryBackground;
  case SemanticColor::TertiaryBackground:
    return colors.tertiaryBackground;

  // Button states
  case SemanticColor::ButtonPrimary:
    return colors.buttonPrimary;
  case SemanticColor::ButtonPrimaryHover:
    return colors.buttonPrimaryHover;
  case SemanticColor::ButtonPrimaryPressed:
    return colors.buttonPrimaryPressed;
  case SemanticColor::ButtonPrimaryDisabled:
    return colors.buttonPrimaryDisabled;

  case SemanticColor::ButtonSecondary:
    return colors.buttonSecondary;
  case SemanticColor::ButtonSecondaryHover:
    return colors.buttonSecondaryHover;
  case SemanticColor::ButtonSecondaryPressed:
    return colors.buttonSecondaryPressed;
  case SemanticColor::ButtonSecondaryDisabled:
    return colors.buttonSecondaryDisabled;

  case SemanticColor::ButtonDestructive:
    return colors.buttonDestructive;
  case SemanticColor::ButtonDestructiveHover:
    return colors.buttonDestructiveHover;
  case SemanticColor::ButtonDestructivePressed:
    return colors.buttonDestructivePressed;

  // Input states
  case SemanticColor::InputBackground:
    return colors.inputBackground;
  case SemanticColor::InputBorder:
    return colors.inputBorder;
  case SemanticColor::InputBorderFocus:
    return colors.inputBorderFocus;
  case SemanticColor::InputBorderError:
    return colors.inputBorderError;
  case SemanticColor::InputPlaceholder:
    return colors.inputPlaceholder;

  // Borders
  case SemanticColor::Border:
    return colors.border;
  case SemanticColor::BorderSubtle:
    return colors.borderSubtle;
  case SemanticColor::BorderStrong:
    return colors.borderStrong;
  case SemanticColor::Separator:
    return colors.separator;
  case SemanticColor::Shadow:
    return colors.shadow;

  // Status colors
  case SemanticColor::StatusBackground:
    return colors.statusBackground;
  case SemanticColor::StatusBorder:
    return colors.statusBackgroundBorder;
  case SemanticColor::StatusHover:
    return colors.statusBackgroundHover;

  case SemanticColor::ErrorBackground:
    return colors.errorBackground;
  case SemanticColor::ErrorBorder:
    return colors.errorBorder;
  case SemanticColor::SuccessBackground:
    return colors.successBackground;
  case SemanticColor::SuccessBorder:
    return colors.successBorder;
  case SemanticColor::WarningBackground:
    return colors.warningBackground;
  case SemanticColor::WarningBorder:
    return colors.warningBorder;

  // Interactive
  case SemanticColor::LinkDefault:
    return colors.linkDefault;
  case SemanticColor::LinkHover:
    return colors.linkHover;
  case SemanticColor::LinkVisited:
    return colors.linkVisited;

  // Special
  case SemanticColor::Focus:
    return colors.focus;
  case SemanticColor::Overlay:
    return colors.overlay;
  case SemanticColor::Tooltip:
    return colors.tooltip;
  case SemanticColor::TooltipText:
    return colors.tooltipText;

  default:
    break;
  }

  return {};
}

QColor ThemeInfo::adjustColorHSL(const QColor &base, int hueShift, float satMult, float lightMult) {
  auto hsl = base.toHsl();

  int newHue = (hsl.hue() + hueShift) % 360;
  if (newHue < 0) newHue += 360;

  int newSat = qBound(0, (int)(hsl.saturation() * satMult), 255);
  int newLight = qBound(0, (int)(hsl.lightness() * lightMult), 255);

  return QColor::fromHsl(newHue, newSat, newLight, hsl.alpha());
}

ThemeInfo ThemeInfo::fromParsed(const ParsedThemeData &scheme) {
  ThemeInfo info;

  // IMPORTANT: most of the semantic colors derived from the palette have been generated
  // but are not used yet, only the main ones are.
  // Eventually we will shit toward using more meaningful semantic colors for particular elements.

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
  info.colors.cyan = scheme.palette.cyan;
  info.colors.mainBackground = scheme.palette.background;

  if (scheme.appearance == "dark") {
    // EXISTING COLORS (your current code)
    info.colors.mainBackground = scheme.palette.background;
    info.colors.border = adjustColorHSL(info.colors.mainBackground, 0, 0.5f, 1.8f);
    info.colors.mainSelectedBackground = adjustColorHSL(info.colors.mainBackground, 0, 1.1f, 1.4f);
    info.colors.mainHoveredBackground = adjustColorHSL(info.colors.mainBackground, 0, 1.0f, 1.3f);
    info.colors.statusBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 1.3f);
    info.colors.statusBackgroundLighter = adjustColorHSL(info.colors.statusBackground, 0, 0.9f, 1.2f);
    info.colors.statusBackgroundHover = adjustColorHSL(info.colors.statusBackground, 0, 1.0f, 1.1f);
    info.colors.statusBackgroundBorder = adjustColorHSL(info.colors.statusBackground, 0, 0.6f, 1.5f);
    info.colors.text = scheme.palette.foreground;
    info.colors.subtext = adjustColorHSL(scheme.palette.foreground, 0, 0.8f, 0.7f);

    // NEW TEXT COLORS
    info.colors.textTertiary = adjustColorHSL(scheme.palette.foreground, 0, 0.6f, 0.5f);
    // ^ Much dimmer for least important text
    info.colors.textDisabled = adjustColorHSL(scheme.palette.foreground, 0, 0.3f, 0.4f);
    // ^ Very desaturated and dim for disabled states
    info.colors.textOnAccent = QColor("#FFFFFF");
    // ^ Always white text on colored buttons in dark theme

    // NEW BACKGROUND LEVELS
    info.colors.secondaryBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.9f, 1.2f);
    // ^ Cards, panels - slightly elevated
    info.colors.tertiaryBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 0.8f);
    // ^ Inset areas, wells - slightly darker

    // PRIMARY BUTTONS (using blue as primary)
    info.colors.buttonPrimary = scheme.palette.blue;
    info.colors.buttonPrimaryHover = adjustColorHSL(scheme.palette.blue, 0, 1.1f, 1.2f);
    // ^ Slightly more saturated and lighter
    info.colors.buttonPrimaryPressed = adjustColorHSL(scheme.palette.blue, 0, 1.2f, 0.8f);
    // ^ More saturated but darker for pressed state
    info.colors.buttonPrimaryDisabled = adjustColorHSL(scheme.palette.blue, 0, 0.3f, 0.6f);
    // ^ Very desaturated and dim

    // SECONDARY BUTTONS (neutral colored)
    info.colors.buttonSecondary = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 1.6f);
    info.colors.buttonSecondaryHover = adjustColorHSL(info.colors.buttonSecondary, 0, 1.0f, 1.2f);
    info.colors.buttonSecondaryPressed = adjustColorHSL(info.colors.buttonSecondary, 0, 1.1f, 0.9f);
    info.colors.buttonSecondaryDisabled = adjustColorHSL(info.colors.buttonSecondary, 0, 0.5f, 0.7f);

    // DESTRUCTIVE BUTTONS (using red)
    info.colors.buttonDestructive = scheme.palette.red;
    info.colors.buttonDestructiveHover = adjustColorHSL(scheme.palette.red, 0, 1.1f, 1.2f);
    info.colors.buttonDestructivePressed = adjustColorHSL(scheme.palette.red, 0, 1.2f, 0.8f);

    // INPUT STATES
    info.colors.inputBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.7f, 1.1f);
    // ^ Slightly lighter than main background
    info.colors.inputBorder = adjustColorHSL(info.colors.mainBackground, 0, 0.6f, 1.5f);
    info.colors.inputBorderFocus = scheme.palette.blue;
    // ^ Use primary color for focus
    info.colors.inputBorderError = scheme.palette.red;
    info.colors.inputPlaceholder = adjustColorHSL(scheme.palette.foreground, 0, 0.5f, 0.6f);

    // BORDER VARIATIONS
    info.colors.borderSubtle = adjustColorHSL(info.colors.border, 0, 0.7f, 0.8f);
    // ^ Even more subtle than regular border
    info.colors.borderStrong = adjustColorHSL(info.colors.border, 0, 1.3f, 1.3f);
    // ^ More prominent border
    info.colors.separator = adjustColorHSL(info.colors.border, 0, 0.5f, 1.0f);
    info.colors.shadow = QColor(0, 0, 0, 80);
    // ^ Semi-transparent black for shadows

    // STATUS BACKGROUNDS (using semantic colors with low opacity effect)
    info.colors.errorBackground = adjustColorHSL(scheme.palette.red, 0, 0.6f, 1.8f);
    info.colors.errorBorder = adjustColorHSL(scheme.palette.red, 0, 0.8f, 1.4f);
    info.colors.successBackground = adjustColorHSL(scheme.palette.green, 0, 0.6f, 1.8f);
    info.colors.successBorder = adjustColorHSL(scheme.palette.green, 0, 0.8f, 1.4f);
    info.colors.warningBackground = adjustColorHSL(scheme.palette.orange, 0, 0.6f, 1.8f);
    info.colors.warningBorder = adjustColorHSL(scheme.palette.orange, 0, 0.8f, 1.4f);

    // LINKS
    info.colors.linkDefault = adjustColorHSL(scheme.palette.blue, 0, 1.0f, 1.3f);
    // ^ Slightly lighter blue for better readability
    info.colors.linkHover = adjustColorHSL(scheme.palette.blue, 0, 1.2f, 1.5f);
    info.colors.linkVisited = adjustColorHSL(scheme.palette.purple, 0, 1.0f, 1.2f);

    // SPECIAL ELEMENTS
    info.colors.focus = scheme.palette.blue;
    // ^ Use primary color for focus rings
    info.colors.overlay = QColor(0, 0, 0, 120);
    // ^ Semi-transparent black for modal overlays
    info.colors.tooltip = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 2.0f);
    info.colors.tooltipText = scheme.palette.foreground;

  } else {
    // LIGHT THEME - Similar logic but inverted lightness relationships

    // EXISTING COLORS (your current code)
    info.colors.mainBackground = scheme.palette.background;
    info.colors.border = adjustColorHSL(info.colors.mainBackground, 0, 0.6f, 0.75f);
    info.colors.mainSelectedBackground = adjustColorHSL(info.colors.mainBackground, 0, 1.2f, 0.9f);
    info.colors.mainHoveredBackground = adjustColorHSL(info.colors.mainBackground, 0, 1.1f, 0.95f);
    info.colors.statusBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.9f, 0.92f);
    info.colors.statusBackgroundLighter = adjustColorHSL(info.colors.statusBackground, 0, 0.8f, 0.96f);
    info.colors.statusBackgroundHover = adjustColorHSL(info.colors.statusBackground, 0, 1.1f, 0.88f);
    info.colors.statusBackgroundBorder = adjustColorHSL(info.colors.statusBackground, 0, 0.7f, 0.8f);
    info.colors.text = scheme.palette.foreground;
    info.colors.subtext = adjustColorHSL(scheme.palette.foreground, 0, 0.7f, 1.4f);

    // NEW TEXT COLORS
    info.colors.textTertiary = adjustColorHSL(scheme.palette.foreground, 0, 0.6f, 1.6f);
    info.colors.textDisabled = adjustColorHSL(scheme.palette.foreground, 0, 0.4f, 1.8f);
    info.colors.textOnAccent = QColor("#FFFFFF");
    // ^ White text works on most colored buttons in light theme too

    // NEW BACKGROUND LEVELS
    info.colors.secondaryBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 0.95f);
    // ^ Cards, panels - slightly darker
    info.colors.tertiaryBackground = adjustColorHSL(info.colors.mainBackground, 0, 0.9f, 1.05f);
    // ^ Inset areas - slightly lighter

    // PRIMARY BUTTONS
    info.colors.buttonPrimary = scheme.palette.blue;
    info.colors.buttonPrimaryHover = adjustColorHSL(scheme.palette.blue, 0, 1.1f, 0.9f);
    // ^ More saturated and darker
    info.colors.buttonPrimaryPressed = adjustColorHSL(scheme.palette.blue, 0, 1.2f, 0.8f);
    info.colors.buttonPrimaryDisabled = adjustColorHSL(scheme.palette.blue, 0, 0.3f, 1.5f);

    // SECONDARY BUTTONS
    info.colors.buttonSecondary = adjustColorHSL(info.colors.mainBackground, 0, 0.8f, 0.85f);
    info.colors.buttonSecondaryHover = adjustColorHSL(info.colors.buttonSecondary, 0, 1.0f, 0.8f);
    info.colors.buttonSecondaryPressed = adjustColorHSL(info.colors.buttonSecondary, 0, 1.1f, 0.75f);
    info.colors.buttonSecondaryDisabled = adjustColorHSL(info.colors.buttonSecondary, 0, 0.5f, 1.2f);

    // DESTRUCTIVE BUTTONS
    info.colors.buttonDestructive = scheme.palette.red;
    info.colors.buttonDestructiveHover = adjustColorHSL(scheme.palette.red, 0, 1.1f, 0.9f);
    info.colors.buttonDestructivePressed = adjustColorHSL(scheme.palette.red, 0, 1.2f, 0.8f);

    // INPUT STATES
    info.colors.inputBackground = QColor("#FFFFFF");
    // ^ Pure white for inputs in light theme
    info.colors.inputBorder = adjustColorHSL(info.colors.mainBackground, 0, 0.5f, 0.7f);
    info.colors.inputBorderFocus = scheme.palette.blue;
    info.colors.inputBorderError = scheme.palette.red;
    info.colors.inputPlaceholder = adjustColorHSL(scheme.palette.foreground, 0, 0.5f, 1.5f);

    // BORDER VARIATIONS
    info.colors.borderSubtle = adjustColorHSL(info.colors.border, 0, 0.7f, 1.2f);
    info.colors.borderStrong = adjustColorHSL(info.colors.border, 0, 1.2f, 0.6f);
    info.colors.separator = adjustColorHSL(info.colors.border, 0, 0.6f, 1.0f);
    info.colors.shadow = QColor(0, 0, 0, 40);
    // ^ Lighter shadow for light theme

    // STATUS BACKGROUNDS
    info.colors.errorBackground = adjustColorHSL(scheme.palette.red, 0, 0.3f, 1.8f);
    info.colors.errorBorder = adjustColorHSL(scheme.palette.red, 0, 0.7f, 1.2f);
    info.colors.successBackground = adjustColorHSL(scheme.palette.green, 0, 0.3f, 1.8f);
    info.colors.successBorder = adjustColorHSL(scheme.palette.green, 0, 0.7f, 1.2f);
    info.colors.warningBackground = adjustColorHSL(scheme.palette.orange, 0, 0.3f, 1.8f);
    info.colors.warningBorder = adjustColorHSL(scheme.palette.orange, 0, 0.7f, 1.2f);

    // LINKS
    info.colors.linkDefault = adjustColorHSL(scheme.palette.blue, 0, 1.1f, 0.8f);
    info.colors.linkHover = adjustColorHSL(scheme.palette.blue, 0, 1.3f, 0.7f);
    info.colors.linkVisited = adjustColorHSL(scheme.palette.purple, 0, 1.1f, 0.8f);

    // SPECIAL ELEMENTS
    info.colors.focus = scheme.palette.blue;
    info.colors.overlay = QColor(0, 0, 0, 80);
    // ^ Slightly lighter overlay for light theme
    info.colors.tooltip = adjustColorHSL(info.colors.mainBackground, 0, 0.7f, 0.2f);
    // ^ Much darker tooltip in light theme
    info.colors.tooltipText = QColor("#FFFFFF");
    // ^ White text on dark tooltip
  }

  return info;
}

void ThemeService::setTheme(const ThemeInfo &info) {
  m_theme = info;

  double mainInputSize = std::round(m_baseFontPointSize * 1.20);

  /**
   * We try to not use stylesheets directly in most of the app, but some very high level
   * rules can help fix issues that would be hard to fix otherwise.
   */
  auto style = QString(R"(
  		QWidget {
			font-size: %1pt;
		}

		QLineEdit, QTextEdit, QPlainTextEdit {
			background-color: transparent;
			border: none;
		}
		QLineEdit:focus[form-input="true"] {
			border-color: %2;
		}
		QTextEdit {
			font-family: monospace;
		}

	   QLineEdit[search-input="true"] {
			font-size: %3pt;
		}

		QScrollArea, 
		QScrollArea > QWidget,
		QScrollArea > QWidget > QWidget
		{ background: transparent; }
		)")
                   .arg(m_baseFontPointSize)
                   .arg(info.colors.border.name())
                   .arg(mainInputSize);

  auto palette = QApplication::palette();

  palette.setBrush(QPalette::WindowText, info.colors.text);
  palette.setBrush(QPalette::Text, info.colors.text);
  palette.setBrush(QPalette::Link, info.colors.linkDefault);
  palette.setBrush(QPalette::LinkVisited, info.colors.linkVisited);

  QColor placeholderText = info.colors.subtext;

  placeholderText.setAlpha(200);

  OmniPainter painter;

  palette.setBrush(QPalette::PlaceholderText, placeholderText);
  palette.setBrush(QPalette::Highlight, painter.colorBrush(info.colors.blue));
  palette.setBrush(QPalette::HighlightedText, info.colors.text);

  QApplication::setPalette(palette);

  Timer timer;
  qApp->setStyleSheet(style);
  timer.time("Theme changed");

  emit themeChanged(info);
}

void ThemeService::registerBuiltinThemes() {
  for (const auto &scheme : loadColorSchemes()) {
    registerTheme(ThemeInfo::fromParsed(scheme));
  }
}

std::optional<ThemeInfo> ThemeService::findTheme(const QString &name) {
  auto it = std::ranges::find_if(m_themes, [&](auto &&theme) { return theme.id == name; });

  if (it == m_themes.end()) {
    QString normalized = QString("%1.json").arg(name);
    it = std::ranges::find_if(m_themes, [&](auto &&theme) { return theme.id == normalized; });
  }

  if (it == m_themes.end()) return {};

  return *it;
}

void ThemeService::upsertTheme(const ParsedThemeData &data) {
  auto info = ThemeInfo::fromParsed(data);
  auto it = std::ranges::find_if(m_themes, [&](const ThemeInfo &model) { return info.id == model.id; });

  if (it == m_themes.end()) {
    m_themes.emplace_back(info);
    return;
  }

  *it = ThemeInfo::fromParsed(data);
}

void ThemeService::scanThemeDirectories() {
  auto configThemes = Omnicast::configDir() / "themes";
  auto dataThemes = Omnicast::dataDir() / "themes";

  m_themes.clear();
  scanThemeDirectory(configThemes);
  scanThemeDirectory(dataThemes);

  for (const auto dir : Omnicast::xdgDataDirs()) {
    fs::path themeDir = dir / "vicinae" / "themes";
    std::error_code ec;

    if (!fs::is_directory(themeDir, ec)) continue;

    scanThemeDirectory(themeDir);
  }
}

void ThemeService::scanThemeDirectory(const std::filesystem::path &path) {
  std::error_code ec;
  std::stack<std::filesystem::path> dirs;

  dirs.push(path);

  while (!dirs.empty()) {
    auto dir = dirs.top();
    dirs.pop();
    auto it = std::filesystem::directory_iterator(dir, ec);

    for (const auto &entry : it) {
      if (entry.is_directory()) {
        dirs.push(entry.path());
        continue;
      }

      bool isJson = entry.path().string().ends_with(".json");

      if (!isJson) continue;

      QFile file(entry.path());

      if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Theme: failed to open" << entry.path() << "for reading";
        continue;
      }

      QJsonParseError error;
      auto json = QJsonDocument::fromJson(file.readAll(), &error);

      if (error.error != QJsonParseError::NoError) {
        qCritical() << "Failed to parse" << entry.path() << "as JSON: " << error.errorString();
        continue;
      }

      QJsonObject obj = json.object();
      ParsedThemeData theme;

      theme.id = QString::fromStdString(entry.path().filename().string());
      theme.appearance = obj.value("appearance").toString();
      theme.name = obj.value("name").toString();
      theme.description = obj.value("description").toString();

      if (theme.name.isEmpty()) {
        qCritical() << "Ignoring theme" << entry.path() << "=> missing name field";
        continue;
      }

      if (obj.contains("icon")) {
        QString rawIcon = obj.value("icon").toString();

        if (rawIcon.isEmpty()) { qWarning() << "'icon' field specified but empty"; }

        // assuming absolute path
        if (rawIcon.startsWith("/")) {
          theme.icon = rawIcon.toStdString();
        } else {
          theme.icon = dir / rawIcon.toStdString();
        }
      }

      auto colors = obj.value("palette").toObject();

      // TODO: use default value for missing colors
      theme.palette.background = colors.value("background").toString();
      theme.palette.foreground = colors.value("foreground").toString();
      theme.palette.blue = colors.value("blue").toString();
      theme.palette.green = colors.value("green").toString();
      theme.palette.magenta = colors.value("magenta").toString();
      theme.palette.orange = colors.value("orange").toString();
      theme.palette.purple = colors.value("purple").toString();
      theme.palette.red = colors.value("red").toString();
      theme.palette.yellow = colors.value("yellow").toString();
      theme.palette.cyan = colors.value("cyan").toString();

      upsertTheme(theme);

      // use default
    }
  }
}

std::vector<ParsedThemeData> ThemeService::loadColorSchemes() const {
  std::vector<ParsedThemeData> schemes;

  schemes.reserve(2);

  ParsedThemeData lightTheme;

  lightTheme.name = "Vicinae Light";
  lightTheme.description = "Default Vicinae light palette";
  lightTheme.id = "vicinae-light";
  lightTheme.appearance = "light";
  lightTheme.palette = ColorPalette{.background = "#F4F2EE",
                                    .foreground = "#1A1A1A",
                                    .blue = "#1F6FEB",
                                    .green = "#3A9C61",
                                    .magenta = "#A48ED6",
                                    .orange = "#DA8A48",
                                    .purple = "#8374B7",
                                    .red = "#C25C49",
                                    .yellow = "#BFAE78",
                                    .cyan = "#18A5B3"};

  schemes.emplace_back(lightTheme);

  ParsedThemeData darkTheme;

  darkTheme.name = "Vicinae Dark";
  darkTheme.description = "Default Vicinae dark palette";
  darkTheme.id = "vicinae-dark";
  darkTheme.appearance = "dark";
  darkTheme.palette = ColorPalette{.background = "#1A1A1A",
                                   .foreground = "#E8E6E1",
                                   .blue = "#2F6FED",
                                   .green = "#3A9C61",
                                   .magenta = "#BC8CFF",
                                   .orange = "#F0883E",
                                   .purple = "#7267B0",
                                   .red = "#B9543B",
                                   .yellow = "#BFAE78",
                                   .cyan = "#18A5B3"};
  schemes.emplace_back(darkTheme);

  return schemes;
}

ThemeService &ThemeService::instance() {
  static ThemeService _instance;

  return _instance;
}

double ThemeService::pointSize(TextSize size) const {
  switch (size) {
  case TextSize::TextRegular:
    return m_baseFontPointSize;
  case TextSize::TextTitle:
    return m_baseFontPointSize * 1.5;
  case TextSize::TextSmaller:
    return m_baseFontPointSize * 0.9;
  }

  return m_baseFontPointSize;
}

void ThemeService::setFontBasePointSize(double pointSize) { m_baseFontPointSize = pointSize; }

void ThemeService::reloadCurrentTheme() { setTheme(m_theme.id); }

std::optional<ThemeInfo> ThemeService::theme(const QString &name) const {
  for (const auto &info : m_themes) {
    if (info.name == name) { return info; }
  }

  return std::nullopt;
}

bool ThemeService::setTheme(const QString &name) {
  if (auto theme = findTheme(name)) {
    setTheme(*theme);
    return true;
  }

  return false;
}

const ThemeInfo &ThemeService::theme() const { return m_theme; }

ColorLike ThemeService::getTintColor(SemanticColor tint) const { return m_theme.resolveTint(tint); }

ThemeService::ThemeService() {
  registerBuiltinThemes();
  scanThemeDirectories();
  setTheme("vicinae-dark");
}
