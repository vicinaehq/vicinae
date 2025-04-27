#include "theme.hpp"
#include "ui/omni-painter.hpp"
#include <QLinearGradient>

void ThemeService::setTheme(const ThemeInfo &info) {
  _theme = info;

  /*
   *		QWidget {
                  font-family: 'SF Pro Text';
                  font-size: 10pt;
                  font-weight: lighter;
                  letter-spacing: -0.5px;
          }
          */

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

std::vector<ColorScheme> ThemeService::loadColorSchemes() const {
  // XXX - Later on we may want to load those from somewhere on the filesystem (I guess)
  std::vector<ColorScheme> schemes;

  schemes.reserve(10);

  ColorScheme solarizedOsaka;

  // cyan = "0x29a298"
  solarizedOsaka.name = "Solarized Osaka";
  solarizedOsaka.blue = "#268bd3";
  solarizedOsaka.green = "#849900";
  solarizedOsaka.purple = "#d23681";
  solarizedOsaka.magenta = "#d23681";
  solarizedOsaka.red = "#db302d";
  solarizedOsaka.yellow = "#b28500";
  solarizedOsaka.orange = "#b28500";
  solarizedOsaka.background = "#001419";
  solarizedOsaka.foreground = "#9eabac";

  schemes.emplace_back(solarizedOsaka);

  // 2. Monokai
  schemes.emplace_back(ColorScheme{
      "Monokai",
      QColor("#272822"), // Background
      QColor("#f8f8f2"), // Foreground
      QColor("#66d9ef"), // Blue
      QColor("#a6e22e"), // Green
      QColor("#f92672"), // Magenta
      QColor("#fd971f"), // Orange
      QColor("#ae81ff"), // Purple
      QColor("#f92672"), // Red
      QColor("#e6db74")  // Yellow
  });

  // 3. Dracula
  schemes.emplace_back(ColorScheme{
      "Dracula",
      QColor("#282a36"), // Background
      QColor("#f8f8f2"), // Foreground
      QColor("#8be9fd"), // Blue
      QColor("#50fa7b"), // Green
      QColor("#ff79c6"), // Magenta
      QColor("#ffb86c"), // Orange
      QColor("#bd93f9"), // Purple
      QColor("#ff5555"), // Red
      QColor("#f1fa8c")  // Yellow
  });

  // 4. Nord
  schemes.emplace_back(ColorScheme{
      "Nord",
      QColor("#2e3440"), // Background
      QColor("#d8dee9"), // Foreground
      QColor("#81a1c1"), // Blue
      QColor("#a3be8c"), // Green
      QColor("#b48ead"), // Magenta
      QColor("#d08770"), // Orange
      QColor("#b48ead"), // Purple
      QColor("#bf616a"), // Red
      QColor("#ebcb8b")  // Yellow
  });

  // 5. Gruvbox Dark
  schemes.emplace_back(ColorScheme{
      "Gruvbox Dark",
      QColor("#282828"), // Background
      QColor("#ebdbb2"), // Foreground
      QColor("#83a598"), // Blue
      QColor("#b8bb26"), // Green
      QColor("#d3869b"), // Magenta
      QColor("#fe8019"), // Orange
      QColor("#d3869b"), // Purple
      QColor("#fb4934"), // Red
      QColor("#fabd2f")  // Yellow
  });

  // 6. One Dark
  schemes.emplace_back(ColorScheme{
      "One Dark",
      QColor("#282c34"), // Background
      QColor("#abb2bf"), // Foreground
      QColor("#61afef"), // Blue
      QColor("#98c379"), // Green
      QColor("#c678dd"), // Magenta
      QColor("#d19a66"), // Orange
      QColor("#c678dd"), // Purple
      QColor("#e06c75"), // Red
      QColor("#e5c07b")  // Yellow
  });

  // 7. Tokyo Night
  schemes.emplace_back(ColorScheme{
      "Tokyo Night",
      QColor("#1a1b26"), // Background
      QColor("#a9b1d6"), // Foreground
      QColor("#7aa2f7"), // Blue
      QColor("#9ece6a"), // Green
      QColor("#bb9af7"), // Magenta
      QColor("#ff9e64"), // Orange
      QColor("#9d7cd8"), // Purple
      QColor("#f7768e"), // Red
      QColor("#e0af68")  // Yellow
  });

  // 8. Catppuccin
  schemes.emplace_back(ColorScheme{
      "Catppuccin",
      QColor("#1e1e2e"), // Background
      QColor("#cdd6f4"), // Foreground
      QColor("#89b4fa"), // Blue
      QColor("#a6e3a1"), // Green
      QColor("#f5c2e7"), // Magenta
      QColor("#fab387"), // Orange
      QColor("#cba6f7"), // Purple
      QColor("#f38ba8"), // Red
      QColor("#f9e2af")  // Yellow
  });

  // 9. Github Dark
  schemes.emplace_back(ColorScheme{
      "Github Dark",
      QColor("#0d1117"), // Background
      QColor("#c9d1d9"), // Foreground
      QColor("#58a6ff"), // Blue
      QColor("#3fb950"), // Green
      QColor("#d2a8ff"), // Magenta
      QColor("#f0883e"), // Orange
      QColor("#bc8cff"), // Purple
      QColor("#f85149"), // Red
      QColor("#e3b341")  // Yellow
  });

  // 10. Material Deep Ocean
  schemes.emplace_back(ColorScheme{
      "Material Deep Ocean",
      QColor("#0f111a"), // Background
      QColor("#8f93a2"), // Foreground
      QColor("#82aaff"), // Blue
      QColor("#c3e88d"), // Green
      QColor("#ff9cac"), // Magenta
      QColor("#f78c6c"), // Orange
      QColor("#c792ea"), // Purple
      QColor("#ff5370"), // Red
      QColor("#ffcb6b")  // Yellow
  });

  return schemes;
}
