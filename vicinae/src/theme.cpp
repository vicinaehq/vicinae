#include "theme.hpp"
#include "template-engine/template-engine.hpp"
#include "timer.hpp"
#include "ui/omni-painter/omni-painter.hpp"
#include "vicinae.hpp"
#include <QLinearGradient>
#include <filesystem>
#include <QStyleHints>
#include <stack>
#include <system_error>
#include <stack>

namespace fs = std::filesystem;

// Helper function to get color with fallback - make inline for performance
inline static QColor getColorWithFallback(const QColor &primary, const QColor &fallback) {
  return primary.isValid() ? primary : fallback;
}

QColor ThemeInfo::resolveTint(SemanticColor tint) const {
  switch (tint) {
  // Basic palette
  case SemanticColor::Blue:
    return colors.base0D;
  case SemanticColor::Green:
    return colors.base0B;
  case SemanticColor::Magenta:
    return colors.base0E;
  case SemanticColor::Orange:
    return colors.base09;
  case SemanticColor::Purple:
    return colors.base0F;
  case SemanticColor::Red:
    return colors.base08;
  case SemanticColor::Yellow:
    return colors.base0A;
  case SemanticColor::Cyan:
    return colors.base0C;

  // Text colors mapped to base16
  case SemanticColor::TextPrimary:
    return colors.base05;
  case SemanticColor::TextSecondary:
    return colors.base04;
  case SemanticColor::TextTertiary:
    return colors.base03;
  case SemanticColor::TextDisabled:
    return colors.base02;
  case SemanticColor::TextOnAccent:
    return colors.base05;
  case SemanticColor::TextError:
    return colors.base08;
  case SemanticColor::TextSuccess:
    return colors.base0B;
  case SemanticColor::TextWarning:
    return colors.base0A;

  // Backgrounds
  case SemanticColor::MainBackground:
    return colors.base00;
  case SemanticColor::MainHoverBackground:
    return colors.base01;
  case SemanticColor::MainSelectedBackground:
    return colors.base02;
  case SemanticColor::SecondaryBackground:
    return colors.base01;
  case SemanticColor::TertiaryBackground:
    return colors.base02;

  // Button states - grouped by button type
  case SemanticColor::ButtonPrimary:
    return colors.base0D;
  case SemanticColor::ButtonPrimaryHover:
    return colors.base0C;
  case SemanticColor::ButtonPrimaryPressed:
    return colors.base0E;
  case SemanticColor::ButtonPrimaryDisabled:
    return colors.base02;

  case SemanticColor::ButtonSecondary:
    return colors.base05;
  case SemanticColor::ButtonSecondaryHover:
    return colors.base04;
  case SemanticColor::ButtonSecondaryPressed:
    return colors.base03;
  case SemanticColor::ButtonSecondaryDisabled:
    return colors.base02;

  case SemanticColor::ButtonDestructive:
    return colors.base08;
  case SemanticColor::ButtonDestructiveHover:
    return colors.base09;
  case SemanticColor::ButtonDestructivePressed:
    return colors.base0A;

  // Input states
  case SemanticColor::InputBackground:
    return colors.base00;
  case SemanticColor::InputBorder:
    return colors.base03;
  case SemanticColor::InputBorderFocus:
    return colors.base0D;
  case SemanticColor::InputBorderError:
    return colors.base08;
  case SemanticColor::InputPlaceholder:
    return colors.base03;

  // Borders
  case SemanticColor::Border:
    return colors.base03;
  case SemanticColor::BorderSubtle:
    return colors.base02;
  case SemanticColor::BorderStrong:
    return colors.base04;
  case SemanticColor::Separator:
    return colors.base02;
  case SemanticColor::Shadow:
    return colors.base01;

  // Status colors
  case SemanticColor::StatusBackground:
    return colors.base01;
  case SemanticColor::StatusBorder:
    return colors.base03;
  case SemanticColor::StatusHover:
    return colors.base02;

  case SemanticColor::ErrorBackground:
    return colors.base08;
  case SemanticColor::ErrorBorder:
    return colors.base09;
  case SemanticColor::SuccessBackground:
    return colors.base0B;
  case SemanticColor::SuccessBorder:
    return colors.base0A;
  case SemanticColor::WarningBackground:
    return colors.base0A;
  case SemanticColor::WarningBorder:
    return colors.base09;

  // Interactive
  case SemanticColor::LinkDefault:
    return colors.base0D;
  case SemanticColor::LinkHover:
    return colors.base0C;
  case SemanticColor::LinkVisited:
    return colors.base0E;

  // Special
  case SemanticColor::Focus:
    return colors.base0D;
  case SemanticColor::Overlay:
    return colors.base01;
  case SemanticColor::Tooltip:
    return colors.base01;
  case SemanticColor::TooltipText:
    return colors.base05;

  default:
    return {};
  }
}

ThemeInfo ThemeInfo::fromParsed(const ParsedThemeData &scheme) {
  ThemeInfo info;

  // Set basic theme information
  info.id = scheme.id;
  info.name = scheme.name;
  info.appearance = scheme.appearance;
  info.icon = scheme.icon;
  info.description = scheme.description;

  // Copy all base16 colors
  info.colors.base00 = scheme.palette.base00;
  info.colors.base01 = scheme.palette.base01;
  info.colors.base02 = scheme.palette.base02;
  info.colors.base03 = scheme.palette.base03;
  info.colors.base04 = scheme.palette.base04;
  info.colors.base05 = scheme.palette.base05;
  info.colors.base06 = scheme.palette.base06;
  info.colors.base07 = scheme.palette.base07;
  info.colors.base08 = scheme.palette.base08;
  info.colors.base09 = scheme.palette.base09;
  info.colors.base0A = scheme.palette.base0A;
  info.colors.base0B = scheme.palette.base0B;
  info.colors.base0C = scheme.palette.base0C;
  info.colors.base0D = scheme.palette.base0D;
  info.colors.base0E = scheme.palette.base0E;
  info.colors.base0F = scheme.palette.base0F;

  // All colors are now directly mapped from base16 palette

  return info;
}

void ThemeService::setTheme(const ThemeInfo &info) {
  m_theme = info;

  double mainInputSize = std::round(m_baseFontPointSize * 1.20);

  TemplateEngine engine;

  engine.setVar("FONT_SIZE", QString::number(m_baseFontPointSize));
  engine.setVar("INPUT_BORDER_COLOR", theme().resolveTint(SemanticColor::Border).name());
  engine.setVar("INPUT_FOCUS_BORDER_COLOR", theme().resolveTint(SemanticColor::InputBorderFocus).name());
  engine.setVar("SEARCH_FONT_SIZE", QString::number(mainInputSize));

  /**
   * We try to not use stylesheets directly in most of the app, but some very high level
   * rules can help fix issues that would be hard to fix otherwise.
   */
  auto style = engine.build(R"(
  		QWidget {
			font-size: {FONT_SIZE}pt;
		}

		QLineEdit, QTextEdit, QPlainTextEdit {
			background-color: transparent;
			border: none;
 		}

		QWidget[form-input="true"] {
		    border: 2px solid {INPUT_BORDER_COLOR};
			border-radius: 5px;
		}

		QLineEdit[form-input="true"]:focus {
			border-color: {INPUT_FOCUS_BORDER_COLOR};
		}

		QPlainTextEdit[form-input="true"]:focus {
			border-color: {INPUT_FOCUS_BORDER_COLOR};
		}

		QLineEdit[search-input="true"] {
			font-size: {SEARCH_FONT_SIZE}pt;
		}

		QScrollArea,
		QScrollArea > QWidget,
		QScrollArea > QWidget > QWidget {
			background: transparent;
		}
		)");

  auto palette = QApplication::palette();

  palette.setBrush(QPalette::WindowText, info.colors.base05);
  palette.setBrush(QPalette::Text, info.colors.base05);
  palette.setBrush(QPalette::Link, theme().resolveTint(SemanticColor::LinkDefault));
  palette.setBrush(QPalette::LinkVisited, theme().resolveTint(SemanticColor::LinkVisited));

  QColor placeholderText = info.colors.base05;

  OmniPainter painter;

  palette.setBrush(QPalette::PlaceholderText, placeholderText);
  palette.setBrush(QPalette::Highlight, painter.colorBrush(info.colors.base0D));
  palette.setBrush(QPalette::HighlightedText, theme().resolveTint(SemanticColor::TextPrimary));

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
      extractColorPalette(theme.palette, colors);

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
  lightTheme.palette = ColorPalette{.base00 = "#F4F2EE",
                                    .base05 = "#1A1A1A",
                                    .base08 = "#C25C49",
                                    .base09 = "#DA8A48",
                                    .base0A = "#BFAE78",
                                    .base0B = "#3A9C61",
                                    .base0C = "#18A5B3",
                                    .base0D = "#1F6FEB",
                                    .base0E = "#A48ED6",
                                    .base0F = "#8374B7"};

  schemes.emplace_back(lightTheme);

  ParsedThemeData darkTheme;

  darkTheme.name = "Vicinae Dark";
  darkTheme.description = "Default Vicinae dark palette";
  darkTheme.id = "vicinae-dark";
  darkTheme.appearance = "dark";
  darkTheme.palette = ColorPalette{.base00 = "#1A1A1A",
                                   .base05 = "#E8E6E1",
                                   .base08 = "#B9543B",
                                   .base09 = "#F0883E",
                                   .base0A = "#BFAE78",
                                   .base0B = "#3A9C61",
                                   .base0C = "#18A5B3",
                                   .base0D = "#2F6FED",
                                   .base0E = "#BC8CFF",
                                   .base0F = "#7267B0"};
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

// Helper function implementations
bool ThemeService::validateThemeData(ParsedThemeData &theme, const QJsonObject &obj,
                                     const std::filesystem::path &entry, const std::filesystem::path &dir) {
  theme.id = QString::fromStdString(entry.filename().string());
  theme.appearance = obj.value("appearance").toString();
  theme.name = obj.value("name").toString();
  theme.description = obj.value("description").toString();

  if (theme.name.isEmpty()) {
    qCritical() << "Ignoring theme" << entry << "=> missing name field";
    return false;
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

  return true;
}

void ThemeService::extractColorPalette(ColorPalette &palette, const QJsonObject &colors) {
  auto extractColor = [&](const QString &key) { return colors.value(key).toString(); };

  // Extract all base16 colors
  palette.base00 = extractColor("base00");
  palette.base01 = extractColor("base01");
  palette.base02 = extractColor("base02");
  palette.base03 = extractColor("base03");
  palette.base04 = extractColor("base04");
  palette.base05 = extractColor("base05");
  palette.base06 = extractColor("base06");
  palette.base07 = extractColor("base07");
  palette.base08 = extractColor("base08");
  palette.base09 = extractColor("base09");
  palette.base0A = extractColor("base0A");
  palette.base0B = extractColor("base0B");
  palette.base0C = extractColor("base0C");
  palette.base0D = extractColor("base0D");
  palette.base0E = extractColor("base0E");
  palette.base0F = extractColor("base0F");
}
