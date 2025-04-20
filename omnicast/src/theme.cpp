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
