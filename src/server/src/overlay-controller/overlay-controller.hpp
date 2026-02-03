#pragma once
#include "common.hpp"
#include "common/qt.hpp"
#include "ui/overlay/overlay.hpp"
#include <qobject.h>

class OverlayController : public QObject, NonCopyable {
  Q_OBJECT

  ApplicationContext *m_ctx = nullptr;
  QObjectUniquePtr<OverlayView> m_current = nullptr;

public:
  bool hasOverlay() const { return m_current.get(); }

  void setCurrent(OverlayView *view) {
    view->setContext(m_ctx);
    emit currentOverlayChanged(view);
    m_current.reset(view);
  }

  void dismissCurrent() {
    emit currentOverlayDismissed();
    m_current.reset();
  }

  OverlayController(ApplicationContext *ctx) : m_ctx(ctx) {}

signals:
  void currentOverlayChanged(OverlayView *view) const;
  void currentOverlayDismissed() const;
};
