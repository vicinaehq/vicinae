#pragma once
#include <QObject>
#include <QRect>
#include <QWindow>
#include <qqmlregistration.h>
#include "services/background-effect/background-effect-manager.hpp"

class QQuickItem;
class QQuickWindow;

class BackgroundEffectAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(int radius READ radius WRITE setRadius NOTIFY radiusChanged)
  Q_PROPERTY(QRect region READ region WRITE setRegion NOTIFY regionChanged)

public:
  explicit BackgroundEffectAttached(QObject *parent);

  bool enabled() const { return m_enabled; }
  void setEnabled(bool value);

  int radius() const { return m_radius; }
  void setRadius(int value);

  QRect region() const { return m_region; }
  void setRegion(const QRect &value);

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
};

class BackgroundEffect : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(BackgroundEffect)
  QML_UNCREATABLE("")
  QML_ATTACHED(BackgroundEffectAttached)

public:
  static BackgroundEffectAttached *qmlAttachedProperties(QObject *object) {
    return new BackgroundEffectAttached(object);
  }

  static void setManager(BackgroundEffectManager *mgr) { s_manager = mgr; }
  static BackgroundEffectManager *manager() { return s_manager; }

private:
  static inline BackgroundEffectManager *s_manager = nullptr;
};
