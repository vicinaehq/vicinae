#pragma once
#include <QColor>
#include <QObject>
#include <QString>
#include <QWindow>
#include <qqmlregistration.h>

class QQuickItem;
class QQuickWindow;

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
    long animationBehavior = 0;
  };

  void apply();
  void revert();
  void trackWindow(QWindow *window);
  void onWindowChanged(QQuickWindow *window);
  bool eventFilter(QObject *obj, QEvent *event) override;

  QQuickItem *m_item = nullptr;
  QWindow *m_window = nullptr;
  bool m_enabled = false;
  int m_cornerRadius = 0;
  bool m_blurEnabled = false;
  QString m_material;
  QColor m_borderColor;
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

// The QML window must use Qt.Tool (or Qt.Popup / Qt.ToolTip / Qt.SplashScreen)
// so Qt instantiates a QNSPanel. Qt.Dialog and Qt.Window produce a plain
// QNSWindow, where the nonactivating styleMask is silently a no-op.
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

  // Show the launcher panel placed on the cursor's screen without AppKit's reveal-time slide:
  // beginShow() hides and positions it before it is shown, finishShow() reveals it once settled.
  Q_INVOKABLE void beginShow(qreal yFraction);
  Q_INVOKABLE void finishShow(qreal yFraction);

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
  int m_windowLevel = 0;
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
  // Mirrors AppKit's NSWindowLevel constants.
  enum WindowLevel { Normal = 0, Floating = 3, Status = 25, PopUpMenu = 101 };
  Q_ENUM(WindowLevel)

  static MacOSPanelAttached *qmlAttachedProperties(QObject *object) { return new MacOSPanelAttached(object); }
};

void macosSetAccessoryActivationPolicy();
void macosActivateApp();
