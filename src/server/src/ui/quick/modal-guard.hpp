#pragma once
#include <unordered_map>
#include <QObject>
#include <QtQml/qqmlregistration.h>

class QQuickItem;

/**
 * Pointer handlers behind a modal popup still receive taps: once anything inside the popup accepts the
 * press, the delivery agent only visits handlers below and skips the overlay's modal filter. The content
 * sharing a window with the overlay is switched off for as long as a modal shows there.
 */
class ModalGuard : public QObject {
  Q_OBJECT
  QML_ELEMENT
  QML_SINGLETON

public:
  explicit ModalGuard(QObject *parent = nullptr) : QObject(parent) {}

  Q_INVOKABLE void block(QQuickItem *overlay);
  Q_INVOKABLE void unblock(QQuickItem *overlay);

private:
  static void setContentEnabled(QQuickItem *overlay, bool enabled);

  std::unordered_map<QQuickItem *, int> m_depth;
};
