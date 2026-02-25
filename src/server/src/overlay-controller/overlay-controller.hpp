#pragma once
#include "common.hpp"
#include "common/qt.hpp"
#include <qobject.h>

class OverlayController : public QObject, NonCopyable {
  Q_OBJECT

signals:
  void overlayChanged();

public:
  OverlayController(ApplicationContext *ctx) : m_ctx(ctx) {}

  bool hasOverlay() const { return m_current.get(); }
  QObject *current() const { return m_current.get(); }

  void setCurrent(QObject *host) {
    m_current.reset(host);
    emit overlayChanged();
  }

  void dismissCurrent() {
    m_current.reset();
    emit overlayChanged();
  }

private:
  ApplicationContext *m_ctx = nullptr;
  QObjectUniquePtr<QObject> m_current = nullptr;
};
