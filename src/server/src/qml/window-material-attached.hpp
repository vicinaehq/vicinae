#pragma once
#include <QObject>
#include <QRect>
#include <QWindow>
#include <qqmlregistration.h>
#include "services/window-material/window-material-manager.hpp"

class QQuickItem;
class QQuickWindow;

class WindowMaterialAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(int radius READ radius WRITE setRadius NOTIFY radiusChanged)
  Q_PROPERTY(QRect region READ region WRITE setRegion NOTIFY regionChanged)
  Q_PROPERTY(bool supportsRegionalBlur READ supportsRegionalBlur CONSTANT)

public:
  explicit WindowMaterialAttached(QObject *parent);

  bool enabled() const { return m_enabled; }
  void setEnabled(bool value);

  int radius() const { return m_radius; }
  void setRadius(int value);

  QRect region() const { return m_region; }
  void setRegion(const QRect &value);

  static bool supportsRegionalBlur();

signals:
  void enabledChanged();
  void radiusChanged();
  void regionChanged();

private:
  void apply();
  void trackWindow(QWindow *window);
  void onWindowChanged(QQuickWindow *window);
  bool eventFilter(QObject *obj, QEvent *event) override;

  QRect effectiveRegion() const;

  QQuickItem *m_item = nullptr;
  QWindow *m_window = nullptr;
  bool m_enabled = false;
  int m_radius = 0;
  QRect m_region;
  bool m_hasExplicitRegion = false;
  bool m_surfaceReady = false;
  bool m_applied = false;
};

class WindowMaterial : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(WindowMaterial)
  QML_UNCREATABLE("")
  QML_ATTACHED(WindowMaterialAttached)

public:
  static WindowMaterialAttached *qmlAttachedProperties(QObject *object) {
    return new WindowMaterialAttached(object);
  }

  static void setManager(WindowMaterialManager *mgr) { s_manager = mgr; }
  static WindowMaterialManager *manager() { return s_manager; }

private:
  static inline WindowMaterialManager *s_manager = nullptr;
};
