#pragma once
#include "qml/image-url.hpp"
#include "ui/action-pannel/action-panel-view.hpp"
#include "ui/image/url.hpp"
#include <functional>

/**
 * Action panel view that records a keyboard shortcut inline, in place of the
 * usual action list. The host provides persistence through the accept handler,
 * which receives the serialized shortcut (empty when cleared).
 */
class ShortcutRecorderPanelView : public ActionPanelView {
  Q_OBJECT
  Q_PROPERTY(QString title READ title CONSTANT)
  Q_PROPERTY(ImageUrl icon READ icon CONSTANT)
  Q_PROPERTY(QString shortcutId READ shortcutId CONSTANT)
  Q_PROPERTY(QString currentShortcut READ currentShortcut CONSTANT)

public:
  Q_INVOKABLE void accept(int key, int modifiers);
  Q_INVOKABLE void clear();
  Q_INVOKABLE void requestClose();

  using AcceptHandler = std::function<void(const QString &serialized)>;

  ShortcutRecorderPanelView(const QString &title, const ImageURL &icon, const QString &shortcutId,
                            const QString &currentShortcut, QObject *parent = nullptr);

  QString title() const { return m_title; }
  ImageUrl icon() const { return ImageUrl(m_icon); }
  QString shortcutId() const { return m_shortcutId; }
  QString currentShortcut() const { return m_currentShortcut; }

  void setAcceptHandler(AcceptHandler handler) { m_onAccept = std::move(handler); }

  QUrl componentUrl() const override;
  QVariantMap componentProps() override;

private:
  QString m_title;
  ImageURL m_icon;
  QString m_shortcutId;
  QString m_currentShortcut;
  AcceptHandler m_onAccept;
};
