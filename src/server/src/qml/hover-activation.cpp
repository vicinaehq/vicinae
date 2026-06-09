#include "hover-activation.hpp"
#include <QCursor>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QWindow>

HoverActivation::HoverActivation(QObject *parent) : QObject(parent), m_anchor(QCursor::pos()) {
  if (auto *app = qGuiApp) { app->installEventFilter(this); }
}

void HoverActivation::setActive(bool active) {
  if (m_active == active) return;
  m_active = active;
  emit activeChanged();
}

void HoverActivation::reset() {
  m_anchor = QCursor::pos();
  setActive(false);
}

bool HoverActivation::eventFilter(QObject *watched, QEvent *event) {
  switch (event->type()) {
  case QEvent::MouseMove:
    if (!m_active) {
      auto *me = static_cast<QMouseEvent *>(event); // NOLINT
      if (me->globalPosition().toPoint() != m_anchor) { setActive(true); }
    }
    break;
  case QEvent::Show:
    if (auto *window = qobject_cast<QWindow *>(watched); window && !window->transientParent()) { reset(); }
    break;
  default:
    break;
  }

  return QObject::eventFilter(watched, event);
}
