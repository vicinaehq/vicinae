#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <QLinearGradient>

void ThemeService::setTheme(const ThemeInfo &info) {
  m_theme = info;

  auto style = QString(R"(

		QLineEdit, QTextEdit {
			background-color: transparent;
			border: none;
            font-size: 10pt;
		}
		QLineEdit:focus[form-input="true"] {
			border-color: %1;
		}
		QTextEdit {
			font-family: monospace;
		}

	   QLineEdit[search-input="true"] {
			font-size: 12pt;
		}

		QScrollArea, QScrollArea > QWidget { background: transparent; }

		QLabel[subtext="true"] {
			color: %1;
		}
		)")
                   .arg(info.colors.border.name());

  auto palette = QApplication::palette();

  palette.setBrush(QPalette::WindowText, info.colors.text);
  palette.setBrush(QPalette::Text, info.colors.text);

  QColor placeholderText = info.colors.subtext;

  placeholderText.setAlpha(200);

  OmniPainter painter;

  palette.setBrush(QPalette::PlaceholderText, placeholderText);
  palette.setBrush(QPalette::Highlight, painter.colorBrush(info.colors.blue));
  palette.setBrush(QPalette::HighlightedText, info.colors.text);

  QApplication::setPalette(palette);

  qApp->setStyleSheet(style);

  emit themeChanged(info);
}

void ThemeService::registerBuiltinThemes() {
  for (const auto &scheme : loadColorSchemes()) {
    registerTheme(ThemeInfo::fromParsed(scheme));
  }
}

std::optional<ThemeInfo> ThemeService::findTheme(const QString &name) {
  auto it = std::ranges::find_if(m_themes, [&](const ThemeInfo &model) { return model.name == name; });

  if (it == m_themes.end()) return std::nullopt;

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

void ThemeService::scanThemeDirectories() { scanThemeDirectory(m_userThemeDir); }

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

      upsertTheme(theme);

      // use default
    }
  }
}

std::vector<ParsedThemeData> ThemeService::loadColorSchemes() const {
  // XXX - Later on we may want to load those from somewhere on the filesystem (I guess)
  std::vector<ParsedThemeData> schemes;

  schemes.reserve(2);

  ParsedThemeData lightTheme;

  lightTheme.name = "Omnicast Light";
  lightTheme.description = "Default light theme";
  lightTheme.id = "omnicast-light";
  lightTheme.appearance = "light";
  lightTheme.palette = ColorPalette{.background = "#FAFAFA",
                                    .foreground = "#2D3142",
                                    .blue = "#277FBF",
                                    .green = "#5A8F00",
                                    .magenta = "#9F4FD4",
                                    .orange = "#E16100",
                                    .purple = "#7F3FBF",
                                    .red = "#D63031",
                                    .yellow = "#D68000",
                                    .cyan = "#008B94"};

  schemes.emplace_back(lightTheme);

  ParsedThemeData darkTheme;

  darkTheme.name = "Omnicast Dark";
  darkTheme.description = "Default dark theme";
  darkTheme.id = "omnicast-dark";
  darkTheme.appearance = "dark";
  darkTheme.palette = ColorPalette{.background = "#1C1C1C",
                                   .foreground = "#D0D0D0",
                                   .blue = "#6CA0F7",
                                   .green = "#7ECB6A",
                                   .magenta = "#C678DD",
                                   .orange = "#F4A460",
                                   .purple = "#B19CD9",
                                   .red = "#F85E5E",
                                   .yellow = "#FFCC66",
                                   .cyan = "#66D9EF"};
  schemes.emplace_back(darkTheme);

  return schemes;
}
