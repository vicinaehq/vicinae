#include "ui/views/bridge-view.hpp"
#include <algorithm>
#include <QKeyEvent>
#include "keyboard/keyboard.hpp"
#include "ui/views/section-list-model.hpp"

bool ViewHostBase::inputFilter(QKeyEvent *event) {
  if (event->type() != QEvent::KeyPress || event->isAutoRepeat() ||
      (event->modifiers() & ~Qt::KeypadModifier) != Qt::ControlModifier) {
    return false;
  }

  const Keyboard::KeyPress press(*event);
  const auto candidates = press.candidates();
  const auto digitKey = std::ranges::find_if(
      candidates, [](const Keyboard::Shortcut &c) { return c.key() >= Qt::Key_0 && c.key() <= Qt::Key_9; });
  if (digitKey == candidates.end()) return false;

  const int digit = digitKey->key() - Qt::Key_0;
  const int shortcutIndex = digit == 0 ? 9 : digit - 1;
  auto *model = listModel();
  return model && model->activateQuickAccess(shortcutIndex);
}
