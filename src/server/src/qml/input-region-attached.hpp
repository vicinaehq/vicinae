#pragma once
#include <QObject>
#include <QRect>
#include <QWindow>
#include <qqmlregistration.h>

class QQuickItem;
class QQuickWindow;

class InputRegionAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(QRect region READ region WRITE setRegion NOTIFY regionChanged)

public:
  explicit InputRegionAttached(QObject *parent);

  bool enabled() const { return m_enabled; }
  void setEnabled(bool value);

  QRect region() const { return m_region; }
  void setRegion(const QRect &value);

signals:
  void enabledChanged();
  void regionChanged();

private:
  void apply();
  void trackWindow(QWindow *window);
  void onWindowChanged(QQuickWindow *window);
  bool eventFilter(QObject *obj, QEvent *event) override;

  QQuickItem *m_item = nullptr;
  QWindow *m_window = nullptr;
  bool m_enabled = false;
  QRect m_region;
  bool m_surfaceReady = false;
  bool m_applied = false;
};

class InputRegion : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(InputRegion)
  QML_UNCREATABLE("")
  QML_ATTACHED(InputRegionAttached)

public:
  static InputRegionAttached *qmlAttachedProperties(QObject *object) {
    return new InputRegionAttached(object);
  }
};
