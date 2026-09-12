#pragma once
#include "ui/qml-engine-scope.hpp"
#include "ui/image/image-url.hpp"
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <optional>
#include "builtins/dictation/dictation-session.hpp"

class QQuickWindow;

class HudBridge : public QObject {
  Q_OBJECT
  QML_NAMED_ELEMENT(Hud)
  QML_SINGLETON

public:
  static HudBridge *create(QQmlEngine *engine, QJSEngine *) { return QmlEngineScope::get<HudBridge>(engine); }

private:
  Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
  Q_PROPERTY(QString text READ text NOTIFY contentChanged)
  Q_PROPERTY(ImageUrl icon READ icon NOTIFY contentChanged)
  Q_PROPERTY(bool hasIcon READ hasIcon NOTIFY contentChanged)
  Q_PROPERTY(DictationSession *dictation READ dictation NOTIFY dictationChanged)

public:
  explicit HudBridge(QObject *parent);

  bool visible() const { return m_visible; }
  QString text() const { return m_text; }
  ImageUrl icon() const { return m_icon; }
  bool hasIcon() const { return m_icon.isValid(); }
  DictationSession *dictation() const { return m_dictation; }

  Q_INVOKABLE void registerWindow(QQuickWindow *window);

public slots:
  void show(const QString &title, const std::optional<ImageURL> &icon);
  void showDictation(DictationSession *session);

signals:
  void visibleChanged();
  void contentChanged();
  void dictationChanged();

private:
  void hide();
  void clearDictation();

  bool m_visible = false;
  QString m_text;
  ImageUrl m_icon;
  QTimer m_timer;
  QPointer<QQuickWindow> m_window;
  QPointer<DictationSession> m_dictation;
};
