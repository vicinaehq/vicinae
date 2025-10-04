#include "shortcut-recorder.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include <qevent.h>

ShortcutRecorder::ShortcutRecorder() { setupUI(); }

void ShortcutRecorder::attach(QWidget *widget) {
  clear();
  auto targetPos = widget->mapToGlobal(QPoint(widget->width() / 2 - width() / 2, -(height() + 10)));
  move(targetPos);
  show();
}

void ShortcutRecorder::setupUI() {
  setWindowFlags(Qt::Popup);
  setFixedSize(m_defaultSize);
  setFocusPolicy(Qt::StrongFocus);
  m_closeTimer.setSingleShot(true);
  m_closeTimer.setInterval(m_successDismissTimeout);
  m_text->setText("Recording...");
  m_text->setSize(TextSize::TextSmaller);
  m_text->setAlignment(Qt::AlignHCenter);
  m_indicator->setBackgroundColor(SemanticColor::TertiaryBackground);
  m_indicator->hide();

  auto modifierRequired = [](const Keyboard::Shortcut &shortcut) {
    if (!shortcut.hasMods()) return "A modifier is required";
    return "";
  };

  setValidator(modifierRequired);

  VStack()
      .center()
      .add(VStack().margins(10).spacing(10).add(m_indicator, 0, Qt::AlignHCenter).add(m_text))
      .imbue(this);

  connect(&m_closeTimer, &QTimer::timeout, this, [this]() { close(); });
}

void ShortcutRecorder::setColor(const ColorLike &color) {
  OmniPainter painter;
  QColor bg = painter.resolveColor(color);

  bg.setAlphaF(0.2);
  m_indicator->setColor(color);
  m_indicator->setBackgroundColor(bg);
  m_text->setColor(color);
}

void ShortcutRecorder::setValidator(const Validator &validator) { m_validator = validator; }

void ShortcutRecorder::setError(const QString &text) {
  OmniPainter painter;
  m_text->setText(text);
  setColor(SemanticColor::Red);
}

void ShortcutRecorder::setSuccess() {
  setColor(SemanticColor::Green);
  m_text->setText("Keybind updated");
}

void ShortcutRecorder::hideEvent(QHideEvent *event) {
  m_closeTimer.stop();
  QWidget::hideEvent(event);
}

void ShortcutRecorder::clear() {
  OmniPainter painter;
  m_text->setText("Recording...");
  m_text->setColor(SemanticColor::TextPrimary);
  m_indicator->setBackgroundColor(SemanticColor::TertiaryBackground);
  m_indicator->setColor(SemanticColor::TextPrimary);
  m_indicator->hide();
}

bool ShortcutRecorder::isModKey(Qt::Key key) { return key >= Qt::Key_Shift && key <= Qt::Key_Alt; }

void ShortcutRecorder::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) { return QWidget::keyPressEvent(event); }
  clear();

  Keyboard::Shortcut shortcut(event);

  m_indicator->setShortcut(shortcut);
  m_indicator->show();

  if (m_validator) {
    if (auto error = m_validator(shortcut); !error.isEmpty()) {
      setError(error);
      return;
    }
  }

  if (isModKey(static_cast<Qt::Key>(event->key()))) {
    m_text->setText("Recording...");
    return;
  }

  setSuccess();
  m_closeTimer.start();
  emit shortcutChanged(shortcut);
}
