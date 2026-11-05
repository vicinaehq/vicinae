#pragma once
#include <QColor>
#include <QObject>
#include <QPointer>
#include <QQmlListProperty>
#include <QRectF>
#include <QString>
#include <QWindow>
#include <vector>
#include <qqmlregistration.h>

class QQuickItem;
class QQuickWindow;

class WindowsWindowAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(bool blurEnabled READ blurEnabled WRITE setBlurEnabled NOTIFY blurEnabledChanged)
  Q_PROPERTY(bool decorated READ decorated WRITE setDecorated NOTIFY decoratedChanged)
  Q_PROPERTY(int titlebarHeight READ titlebarHeight WRITE setTitlebarHeight NOTIFY titlebarHeightChanged)
  Q_PROPERTY(QQmlListProperty<QQuickItem> titlebarControls READ titlebarControls CONSTANT)
  Q_PROPERTY(QRectF captionButtonsRect READ captionButtonsRect NOTIFY captionButtonsRectChanged)
  Q_PROPERTY(QString appearance READ appearance WRITE setAppearance NOTIFY appearanceChanged)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
  Q_PROPERTY(QColor captionColor READ captionColor WRITE setCaptionColor NOTIFY captionColorChanged)
  Q_PROPERTY(bool captionTitleVisible READ captionTitleVisible WRITE setCaptionTitleVisible NOTIFY captionTitleVisibleChanged)
  Q_PROPERTY(bool acrylicSupported READ acrylicSupported CONSTANT)
  Q_PROPERTY(int nativeCornerRadius READ nativeCornerRadius CONSTANT)

signals:
  void enabledChanged();
  void blurEnabledChanged();
  void decoratedChanged();
  void titlebarHeightChanged();
  void captionButtonsRectChanged();
  void appearanceChanged();
  void borderColorChanged();
  void captionColorChanged();
  void captionTitleVisibleChanged();

public:
  explicit WindowsWindowAttached(QObject *parent);
  ~WindowsWindowAttached() override;

  bool enabled() const { return m_enabled; }
  void setEnabled(bool value);

  bool blurEnabled() const { return m_blurEnabled; }
  void setBlurEnabled(bool value);

  bool decorated() const { return m_decorated; }
  void setDecorated(bool value);

  int titlebarHeight() const { return m_titlebarHeight; }
  void setTitlebarHeight(int value);

  QQmlListProperty<QQuickItem> titlebarControls();

  QRectF captionButtonsRect() const { return m_captionButtonsRect; }

  // "dark" / "light" pin the DWM material tint; empty follows the system.
  QString appearance() const { return m_appearance; }
  void setAppearance(const QString &value);

  QColor borderColor() const { return m_borderColor; }
  void setBorderColor(const QColor &value);

  QColor captionColor() const { return m_captionColor; }
  void setCaptionColor(const QColor &value);

  bool captionTitleVisible() const { return m_captionTitleVisible; }
  void setCaptionTitleVisible(bool value);

  // DWMWA_SYSTEMBACKDROP_TYPE requires Windows 11 22H2.
  static bool acrylicSupported();

  // DWM only rounds to fixed presets; DWMWCP_ROUND clips at 8dip.
  static int nativeCornerRadius();

  long hitTest(const QPoint &clientPx) const;
  long extendedStyle() const;
  void updateCaptionButtonsRect();
  void extendTitlebar();

private:
  void apply();
  void revert();
  void trackWindow(QWindow *window);
  void onWindowChanged(QQuickWindow *window);
  bool eventFilter(QObject *obj, QEvent *event) override;
  bool extended() const { return m_decorated && m_titlebarHeight > 0; }

  QQuickItem *m_item = nullptr;
  QWindow *m_window = nullptr;
  bool m_enabled = false;
  bool m_blurEnabled = false;
  bool m_decorated = false;
  int m_titlebarHeight = 0;
  std::vector<QPointer<QQuickItem>> m_titlebarControls;
  QRectF m_captionButtonsRect;
  QString m_appearance;
  QColor m_borderColor;
  QColor m_captionColor;
  bool m_captionTitleVisible = true;
  bool m_surfaceReady = false;
  bool m_subclassed = false;
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
