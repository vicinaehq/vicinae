#include "search-bar.hpp"
#include "service-registry.hpp"
#include <qcoreevent.h>
#include <qevent.h>
#include "services/config/config-service.hpp"
#include "template-engine/template-engine.hpp"
#include "theme.hpp"

SearchBar::SearchBar(QWidget *parent) : QLineEdit(parent) {
  auto debounce = new QTimer(this);

  setFrame(false);
  setProperty("search-input", true);
  refreshStyle();
  debounce->setInterval(10);
  debounce->setSingleShot(true);

  connect(debounce, &QTimer::timeout, this, &SearchBar::debounce);
  connect(this, &QLineEdit::textEdited, debounce, [debounce]() { debounce->start(); });
  connect(&ThemeService::instance(), &ThemeService::themeChanged, this, [this]() { refreshStyle(); });
  connect(this, &QLineEdit::textEdited, this, [this]() {
    if (m_inline) {
      auto fm = fontMetrics();
      QString sizedText = text();

      if (sizedText.isEmpty()) sizedText = placeholderText();

      int width = fm.horizontalAdvance(sizedText) + 10;

      setFixedWidth(width);
    }
  });
}

void SearchBar::setInline(bool isInline) {
  m_inline = isInline;

  if (isInline) {
    auto fm = fontMetrics();
    QString sizedText = text();

    if (sizedText.isEmpty()) sizedText = placeholderText();

    int width = fm.horizontalAdvance(sizedText) + 10;

    setFixedWidth(width);
    return;
  }

  setFixedWidth(QWIDGETSIZE_MAX);
}

void SearchBar::debounce() { emit debouncedTextEdited(text()); }

// we will eventually get rid of the keybind part for a better system
bool SearchBar::event(QEvent *event) {
  if (event->type() == QEvent::KeyPress) {
    auto keyEvent = static_cast<QKeyEvent *>(event);

    if (keyEvent->key() == Qt::Key_Backspace && text().isEmpty()) {
      emit pop();
      return true;
    }

    // Emacs-style editing bindings for the search bar
    auto config = ServiceRegistry::instance()->config();
    const bool emacsMode = (config && config->value().keybinding == "emacs");

    if (emacsMode) {
      // Ctrl-based bindings
      if (keyEvent->modifiers() == Qt::ControlModifier) {
        switch (keyEvent->key()) {
        case Qt::Key_A: // beginning of line
          setCursorPosition(0);
          return true;
        case Qt::Key_E: // end of line
          setCursorPosition(text().size());
          return true;
        case Qt::Key_B: // backward char
          cursorBackward(false);
          return true;
        case Qt::Key_F: // forward char
          cursorForward(false);
          return true;
        case Qt::Key_K: { // kill to end of line
          int pos = cursorPosition();
          setText(text().left(pos));
          setCursorPosition(text().size());
          return true;
        }
        case Qt::Key_U: { // kill to beginning of line
          int pos = cursorPosition();
          setText(text().mid(pos));
          setCursorPosition(0);
          return true;
        }
        default:
          break;
        }
      }

      // Alt/Meta-based bindings
      if (keyEvent->modifiers() == Qt::AltModifier) {
        switch (keyEvent->key()) {
        case Qt::Key_B: // backward word
          cursorWordBackward(false);
          return true;
        case Qt::Key_F: // forward word
          cursorWordForward(false);
          return true;
        case Qt::Key_Backspace: // delete word backward
          cursorWordBackward(true);
          insert("");
          return true;
        case Qt::Key_Delete: // delete word forward
        case Qt::Key_D:      // M-d also deletes word forward
          cursorWordForward(true);
          insert("");
          return true;
        default:
          break;
        }
      }
    }
  }

  return QLineEdit::event(event);
}

void SearchBar::refreshStyle() {
  TemplateEngine tmpl;
  double textSize = ThemeService::instance().pointSize(TextSize::TextRegular) * 1.20;
  tmpl.setVar("TEXT_SIZE", QString::number(textSize));
  QString stylesheet = tmpl.build(R"(
	QLineEdit[search-input="true"] {
		font-size: {TEXT_SIZE}pt;
	}
	)");
  setStyleSheet(stylesheet);
}
