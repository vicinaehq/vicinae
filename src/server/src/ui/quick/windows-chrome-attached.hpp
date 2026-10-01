#pragma once
#include <QColor>
#include <QObject>
#include <QString>
#include <QWindow>
#include <qqmlregistration.h>

class QQuickItem;
class QQuickWindow;

class WindowsWindowAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(bool blurEnabled READ blurEnabled WRITE setBlurEnabled NOTIFY blurEnabledChanged)
  Q_PROPERTY(QString appearance READ appearance WRITE setAppearance NOTIFY appearanceChanged)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
  Q_PROPERTY(bool acrylicSupported READ acrylicSupported CONSTANT)
  Q_PROPERTY(int nativeCornerRadius READ nativeCornerRadius CONSTANT)

signals:
  void enabledChanged();
  void blurEnabledChanged();
  void appearanceChanged();
  void borderColorChanged();

public:
  explicit WindowsWindowAttached(QObject *parent);

  bool enabled() const { return m_enabled; }
  void setEnabled(bool value);

  bool blurEnabled() const { return m_blurEnabled; }
  void setBlurEnabled(bool value);

  // "dark" / "light" pin the DWM material tint; empty follows the system.
  QString appearance() const { return m_appearance; }
  void setAppearance(const QString &value);

  QColor borderColor() const { return m_borderColor; }
  void setBorderColor(const QColor &value);

  // DWMWA_SYSTEMBACKDROP_TYPE requires Windows 11 22H2.
  static bool acrylicSupported();

  // DWM only rounds to fixed presets; DWMWCP_ROUND clips at 8dip.
  static int nativeCornerRadius();

private:
  void apply();
  void revert();
  void trackWindow(QWindow *window);
  void onWindowChanged(QQuickWindow *window);
  bool eventFilter(QObject *obj, QEvent *event) override;

  QQuickItem *m_item = nullptr;
  QWindow *m_window = nullptr;
  bool m_enabled = false;
  bool m_blurEnabled = false;
  QString m_appearance;
  QColor m_borderColor;
  bool m_surfaceReady = false;
};

class WindowsWindow : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(WindowsWindow)
  QML_UNCREATABLE("")
  QML_ATTACHED(WindowsWindowAttached)

public:
  static WindowsWindowAttached *qmlAttachedProperties(QObject *object) {
    return new WindowsWindowAttached(object);
  }
};

bool windowsAcrylicSupported();
