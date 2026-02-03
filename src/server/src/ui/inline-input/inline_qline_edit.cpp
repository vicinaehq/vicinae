#include "ui/inline-input/inline_qline_edit.hpp"
#include "template-engine/template-engine.hpp"
#include "theme.hpp"
#include "theme/colors.hpp"
#include <QLineEdit>
#include <QStyle>
#include <qpainter.h>
#include <qpainterpath.h>
#include "theme/theme-file.hpp"

InlineQLineEdit::InlineQLineEdit(const QString &placeholder, QWidget *parent) : QLineEdit(parent) {
  setPlaceholderText(placeholder);
  resizeFromText(placeholder + "...");
  setTextMargins(5, 5, 0, 5);
  setProperty("arg-form-input", true);
  updateStyle();
  connect(this, &QLineEdit::textChanged, this, &InlineQLineEdit::handleTextChanged);
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { updateStyle(); });
}

void InlineQLineEdit::resizeFromText(const QString &s) {
  auto fm = fontMetrics();

  setFixedWidth(fm.boundingRect(s).width() + 15);
}

void InlineQLineEdit::handleTextChanged(const QString &s) {
  const QString &text = s.isEmpty() ? placeholderText() : s;

  resizeFromText(text);
}

void InlineQLineEdit::clearError() { setError(""); }

void InlineQLineEdit::setError(const QString &error) {
  m_error = error;
  setProperty("arg-form-input", error.isEmpty());
  setProperty("error", !error.isEmpty());
  style()->unpolish(this);
  style()->polish(this);
  update();
}

void InlineQLineEdit::updateStyle() {
  TemplateEngine engine;
  auto &themeService = ThemeService::instance();
  auto &theme = ThemeService::instance().theme();

  engine.setVar("FONT_SIZE", QString::number(themeService.pointSize(TextSize::TextRegular)));
  engine.setVar("INPUT_BORDER_COLOR", theme.resolveAsString(SemanticColor::InputBorder));
  engine.setVar("INPUT_FOCUS_BORDER_COLOR", theme.resolveAsString(SemanticColor::InputBorderFocus));
  engine.setVar("INPUT_BORDER_ERROR", theme.resolveAsString(SemanticColor::InputBorderError));

  auto style = engine.build(R"(
  		QLineEdit {
			font-size: {FONT_SIZE}pt;
			background-color: transparent;
			border: 2px solid {INPUT_BORDER_COLOR};
			border-radius: 5px;
		}

		QLineEdit[arg-form-input="true"]:focus {
			border-color: {INPUT_FOCUS_BORDER_COLOR};
		}

		QLineEdit[error="true"] {
			border-color: {INPUT_BORDER_ERROR};
		}
	)");

  setStyleSheet(style);
}
