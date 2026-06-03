#pragma once
#include <QObject>
#include <QWindow>
#include <qqmlregistration.h>
#include "services/shortcut-inhibit/shortcut-inhibit-manager.hpp"

class QQuickItem;
class QQuickWindow;

class ShortcutInhibitorAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
  explicit ShortcutInhibitorAttached(QObject *parent);

  bool enabled() const { return m_enabled; }
  void setEnabled(bool value);

signals:
  void enabledChanged();

private:
  void apply();
  void trackWindow(QWindow *window);
  void onWindowChanged(QQuickWindow *window);
  bool eventFilter(QObject *obj, QEvent *event) override;

  QWindow *targetWindow() const;

  QQuickItem *m_item = nullptr;
  QWindow *m_window = nullptr;
  QWindow *m_inhibited = nullptr;
  bool m_enabled = false;
  bool m_surfaceReady = false;
};

class ShortcutInhibitor : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(ShortcutInhibitor)
  QML_UNCREATABLE("")
  QML_ATTACHED(ShortcutInhibitorAttached)

public:
  static ShortcutInhibitorAttached *qmlAttachedProperties(QObject *object) {
    return new ShortcutInhibitorAttached(object);
  }

  static void setManager(ShortcutInhibitManager *mgr) { s_manager = mgr; }
  static ShortcutInhibitManager *manager() { return s_manager; }

private:
  static inline ShortcutInhibitManager *s_manager = nullptr;
};
