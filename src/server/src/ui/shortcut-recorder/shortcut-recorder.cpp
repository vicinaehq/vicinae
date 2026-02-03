#include "shortcut-recorder.hpp"
#include "layout.hpp"
#include "theme.hpp"
#include <qevent.h>
#include <qnamespace.h>
#include <qwidget.h>

ShortcutRecorder::ShortcutRecorder(QWidget *parent) {
  m_target = parent;
  setupUI();
}

void ShortcutRecorder::attach(QWidget *widget) {
  clear();
  recompute();
}

void ShortcutRecorder::setupUI() {
  setWindowFlags(Qt::Popup);
  setMinimumSize(m_defaultSize);
  setFocusPolicy(Qt::StrongFocus);
  m_closeTimer.setSingleShot(true);
  m_closeTimer.setInterval(m_successDismissTimeout);
  m_text->setText("Recording...");
  m_text->setSize(TextSize::TextSmaller);
  m_text->setWordWrap(true);
  m_text->setAlignment(Qt::AlignHCenter);
  m_indicator->setBackgroundColor(SemanticColor::Background);
  m_indicator->hide();
  m_target->installEventFilter(this);

  auto modifierRequired = [](const Keyboard::Shortcut &shortcut) {
    if (!shortcut.hasMods()) return "A modifier is required";
    return "";
  };

  setValidator(modifierRequired);

  auto layout =
      VStack().margins(10).spacing(5).add(m_indicator, 0, Qt::AlignCenter).add(m_text).buildLayout();

  m_indicator->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  layout->setAlignment(Qt::AlignCenter);
  setLayout(layout);

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
  recompute();
}

void ShortcutRecorder::recompute() {
  updateGeometry();
  adjustSize();
  show();
  auto targetPos = m_target->mapToGlobal(QPoint(m_target->width() / 2 - width() / 2, -(height() + 10)));
  move(targetPos);
}

void ShortcutRecorder::setSuccess() {
  setColor(SemanticColor::Green);
  m_text->setText("Keybind updated");
  recompute();
}

void ShortcutRecorder::hideEvent(QHideEvent *event) {
  m_closeTimer.stop();
  QWidget::hideEvent(event);
}

void ShortcutRecorder::clear() {
  OmniPainter painter;
  m_text->setText("Recording...");
  m_text->setColor(SemanticColor::Foreground);
  m_indicator->setBackgroundColor(SemanticColor::ListItemSelectionBackground);
  m_indicator->setColor(SemanticColor::Foreground);
  m_indicator->hide();
}

bool ShortcutRecorder::isModKey(Qt::Key key) { return key >= Qt::Key_Shift && key <= Qt::Key_Alt; }
bool ShortcutRecorder::isCloseKey(Qt::Key key) { return key == Qt::Key_Escape || key == Qt::Key_Backspace; }

bool ShortcutRecorder::eventFilter(QObject *sender, QEvent *event) {
  if (sender == m_target && event->type() == QEvent::Destroy) { deleteLater(); }

  return Popover::eventFilter(sender, event);
}

void ShortcutRecorder::keyPressEvent(QKeyEvent *event) {
  m_closeTimer.stop();

  Keyboard::Shortcut shortcut(event);

  if (!shortcut.hasMods() && isCloseKey(shortcut.key())) {
    close();
    return;
  }

  clear();

  m_indicator->setShortcut(shortcut);
  m_indicator->show();

  if (m_validator) {
    if (auto error = m_validator(shortcut); !error.isEmpty()) {
      setError(error);
      return;
    }
  }

  if (!shortcut.isValid() || !shortcut.isValidKey() || isModKey(static_cast<Qt::Key>(event->key()))) {
    m_text->setText("Recording...");
    return;
  }

  setSuccess();
  m_closeTimer.start();
  emit shortcutChanged(shortcut);
}
