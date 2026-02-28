#pragma once
#include "image-url.hpp"
#include <QObject>
#include <QTimer>
#include <optional>

class QQuickWindow;

class HudBridge : public QObject {
  Q_OBJECT
  Q_PROPERTY(bool visible READ visible NOTIFY visibleChanged)
  Q_PROPERTY(QString text READ text NOTIFY contentChanged)
  Q_PROPERTY(ImageUrl icon READ icon NOTIFY contentChanged)
  Q_PROPERTY(bool hasIcon READ hasIcon NOTIFY contentChanged)

public:
  explicit HudBridge(QObject *parent = nullptr);

  bool visible() const { return m_visible; }
  QString text() const { return m_text; }
  ImageUrl icon() const { return m_icon; }
  bool hasIcon() const { return m_icon.isValid(); }

  Q_INVOKABLE void registerWindow(QQuickWindow *window);

public slots:
  void show(const QString &title, const std::optional<ImageURL> &icon);

signals:
  void visibleChanged();
  void contentChanged();

private:
  void hide();
  void configureLayerShell(QQuickWindow *window);

  bool m_visible = false;
  QString m_text;
  ImageUrl m_icon;
  QTimer m_timer;
  QQuickWindow *m_window = nullptr;
};
