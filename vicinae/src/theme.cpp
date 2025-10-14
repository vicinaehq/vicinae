#include "theme.hpp"
#include "template-engine/template-engine.hpp"
#include "theme/colors.hpp"
#include "theme/theme-db.hpp"
#include "theme/theme-file.hpp"
#include "timer.hpp"
#include <QLinearGradient>
#include <QStyleHints>
#include <qapplication.h>
#include <qpalette.h>

void ThemeService::setTheme(const ThemeFile &info) {
  m_theme = std::make_unique<ThemeFile>(info);

  double mainInputSize = std::round(m_baseFontPointSize * 1.20);

  TemplateEngine engine;

  engine.setVar("FONT_SIZE", QString::number(m_baseFontPointSize));
  engine.setVar("INPUT_BORDER_COLOR", info.resolve(SemanticColor::InputBorder).name());
  engine.setVar("INPUT_FOCUS_BORDER_COLOR", info.resolve(SemanticColor::InputBorderFocus).name());
  engine.setVar("INPUT_BORDER_ERROR", info.resolve(SemanticColor::InputBorderError).name());
  engine.setVar("STATUS_BACKGROUND", info.resolve(SemanticColor::LighterBackground).name());
  engine.setVar("SEARCH_FONT_SIZE", QString::number(mainInputSize));

  /**
   * We try to not use stylesheets directly in most of the app, but some very high level
   * rules can help fix issues that would be hard to fix otherwise.
   */
  auto style = engine.build(R"(
		QLineEdit, QTextEdit, QPlainTextEdit {
			font-size: {FONT_SIZE}pt;
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

		QLineEdit[arg-form-input="true"]:focus {
			border-color: {INPUT_FOCUS_BORDER_COLOR};
		}

		QLineEdit[error="true"] {
			background-color: {STATUS_BACKGROUND};
			border: 2px solid {INPUT_BORDER_ERROR};
			border-radius: 5px;
		}

		QLineEdit[arg-form-input="true"]:focus {
			border-color: {INPUT_FOCUS_BORDER_COLOR};
		}

		QLineEdit[arg-form-input="true"] {
			background-color: {STATUS_BACKGROUND};
			border: 2px solid {INPUT_BORDER_COLOR};
			border-radius: 5px;
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

  palette.setBrush(QPalette::WindowText, info.resolve(SemanticColor::Foreground));
  palette.setBrush(QPalette::Text, info.resolve(SemanticColor::Foreground));
  palette.setBrush(QPalette::Link, info.resolve(SemanticColor::LinkDefault));
  palette.setBrush(QPalette::LinkVisited, info.resolve(SemanticColor::LinkVisited));
  palette.setBrush(QPalette::PlaceholderText, info.resolve(SemanticColor::InputPlaceholder));
  palette.setBrush(QPalette::Highlight, info.resolve(SemanticColor::TextSelectionBackground));
  palette.setBrush(QPalette::HighlightedText, info.resolve(SemanticColor::TextSelectionForeground));
  palette.setBrush(QPalette::Accent, info.resolve(SemanticColor::TextSelectionBackground));

  QApplication::setPalette(palette);

  Timer timer;
  qApp->setStyleSheet(style);
  timer.time("Theme changed");
  emit themeChanged(info);
}

ThemeService &ThemeService::instance() {
  static ThemeService _instance;
  return _instance;
}

ThemeDatabase &ThemeService::db() { return *m_themeDb; }

ThemeDatabase::ThemeList ThemeService::themes() const { return m_themeDb->themes(); }

const ThemeFile *ThemeService::findTheme(const QString &id) { return m_themeDb->theme(id); }

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

void ThemeService::reloadCurrentTheme() { setTheme(m_theme->id()); }

bool ThemeService::setTheme(const QString &name) {
  if (auto theme = m_themeDb->theme(name)) {
    setTheme(*theme);
    return true;
  }
  return false;
}

const ThemeFile &ThemeService::theme() const { return *m_theme; }

ColorLike ThemeService::getTintColor(SemanticColor tint) const { return m_theme->resolve(tint); }

ThemeService::ThemeService() : m_themeDb(std::make_shared<ThemeDatabase>()) {
  m_themeDb->scan();
  setTheme("vicinae-dark");
  connect(m_themeDb.get(), &ThemeDatabase::themeChanged, this, [this](const ThemeFile &theme) {
    if (m_theme->id() == theme.id()) { setTheme(theme); }
  });
}
