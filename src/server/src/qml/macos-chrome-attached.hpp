#pragma once
#include <QColor>
#include <QObject>
#include <QString>
#include <QWindow>
#include <qqmlregistration.h>

class QQuickItem;
class QQuickWindow;

// Visual chrome for any macOS window: transparent NSWindow + NSVisualEffectView
// blur + corner radius + border. Safe to attach to launcher, settings,
// popups, dropdowns — anything that just wants the look.
class MacOSWindowAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(int cornerRadius READ cornerRadius WRITE setCornerRadius NOTIFY cornerRadiusChanged)
  Q_PROPERTY(bool blurEnabled READ blurEnabled WRITE setBlurEnabled NOTIFY blurEnabledChanged)
  Q_PROPERTY(QString material READ material WRITE setMaterial NOTIFY materialChanged)
  Q_PROPERTY(QColor borderColor READ borderColor WRITE setBorderColor NOTIFY borderColorChanged)
  Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)

signals:
  void enabledChanged();
  void cornerRadiusChanged();
  void blurEnabledChanged();
  void materialChanged();
  void borderColorChanged();
  void borderWidthChanged();

public:
  explicit MacOSWindowAttached(QObject *parent);

  bool enabled() const { return m_enabled; }
  void setEnabled(bool value);

  int cornerRadius() const { return m_cornerRadius; }
  void setCornerRadius(int value);

  bool blurEnabled() const { return m_blurEnabled; }
  void setBlurEnabled(bool value);

  QString material() const { return m_material; }
  void setMaterial(const QString &value);

  QColor borderColor() const { return m_borderColor; }
  void setBorderColor(const QColor &value);

  int borderWidth() const { return m_borderWidth; }
  void setBorderWidth(int value);

private:
  struct Snapshot {
    bool valid = false;
    bool opaque = true;
    void *backgroundColor = nullptr;
    bool hasShadow = true;
  };

  void apply();
  void revert();
  void trackWindow(QWindow *window);
  void onWindowChanged(QQuickWindow *window);
  bool eventFilter(QObject *obj, QEvent *event) override;

  QQuickItem *m_item = nullptr;
  QWindow *m_window = nullptr;
  bool m_enabled = false;
  int m_cornerRadius = 12;
  bool m_blurEnabled = true;
  QString m_material = QStringLiteral("hud");
  QColor m_borderColor = QColor(0, 0, 0, 0);
  int m_borderWidth = 0;
  bool m_surfaceReady = false;
  Snapshot m_snapshot;
};

class MacOSWindow : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(MacOSWindow)
  QML_UNCREATABLE("")
  QML_ATTACHED(MacOSWindowAttached)

public:
  static MacOSWindowAttached *qmlAttachedProperties(QObject *object) {
    return new MacOSWindowAttached(object);
  }
};

// Launcher-panel behavior: nonactivating NSPanel styleMask, high window level,
// all-spaces collection behavior, resignKey signal for click-outside dismissal.
// The QML window must use Qt.Tool (or Qt.Popup / Qt.ToolTip / Qt.SplashScreen)
// so Qt instantiates a QNSPanel under the hood — Qt.Dialog and Qt.Window both
// produce a plain QNSWindow, which makes the nonactivating-panel style mask a
// no-op and lets show() activate the whole app, stealing focus from the
// frontmost application.
class MacOSPanelAttached : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(int windowLevel READ windowLevel WRITE setWindowLevel NOTIFY windowLevelChanged)

signals:
  void enabledChanged();
  void windowLevelChanged();
  void resignKey();

public:
  explicit MacOSPanelAttached(QObject *parent);
  ~MacOSPanelAttached() override;

  bool enabled() const { return m_enabled; }
  void setEnabled(bool value);

  int windowLevel() const { return m_windowLevel; }
  void setWindowLevel(int value);

private:
  struct Snapshot {
    bool valid = false;
    unsigned long styleMask = 0;
    int level = 0;
    unsigned long collectionBehavior = 0;
    bool hidesOnDeactivate = false;
    bool movableByWindowBackground = false;
    bool isPanel = false;
    bool floatingPanel = false;
    bool becomesKeyOnlyIfNeeded = false;
    bool worksWhenModal = false;
  };

  void apply();
  void revert();
  void trackWindow(QWindow *window);
  void onWindowChanged(QQuickWindow *window);
  void installResignKeyObserver(void *nswin);
  void removeResignKeyObserver();
  bool eventFilter(QObject *obj, QEvent *event) override;

  QQuickItem *m_item = nullptr;
  QWindow *m_window = nullptr;
  bool m_enabled = false;
  bool m_surfaceReady = false;
  int m_windowLevel = 3; // NSFloatingWindowLevel
  void *m_resignKeyObserver = nullptr;
  void *m_observedNSWindow = nullptr;
  Snapshot m_snapshot;
};

class MacOSPanel : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(MacOSPanel)
  QML_UNCREATABLE("")
  QML_ATTACHED(MacOSPanelAttached)

public:
  static MacOSPanelAttached *qmlAttachedProperties(QObject *object) { return new MacOSPanelAttached(object); }
};

void macosSetAccessoryActivationPolicy();
void macosActivateApp();
