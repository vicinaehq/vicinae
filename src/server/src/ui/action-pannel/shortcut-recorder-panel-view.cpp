#include "ui/action-pannel/shortcut-recorder-panel-view.hpp"
#include "internal/keyboard/keyboard.hpp"

ShortcutRecorderPanelView::ShortcutRecorderPanelView(const QString &title, const ImageURL &icon,
                                                     const QString &shortcutId,
                                                     const QString &currentShortcut, QObject *parent)
    : ActionPanelView(parent), m_title(title), m_icon(icon), m_shortcutId(shortcutId),
      m_currentShortcut(currentShortcut) {
  setId(shortcutId);
}

QUrl ShortcutRecorderPanelView::componentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/ShortcutRecorderPanel.qml"));
}

QVariantMap ShortcutRecorderPanelView::componentProps() {
  QVariantMap props;
  props[QStringLiteral("view")] = QVariant::fromValue(static_cast<QObject *>(this));
  return props;
}

void ShortcutRecorderPanelView::accept(int key, int modifiers) {
  Keyboard::Shortcut const shortcut(static_cast<Qt::Key>(key), static_cast<Qt::KeyboardModifiers>(modifiers));
  if (!shortcut.isValid()) return;

  m_currentShortcut = shortcut.toString();
  if (m_onAccept) m_onAccept(m_currentShortcut);
}

void ShortcutRecorderPanelView::clear() {
  m_currentShortcut.clear();
  if (m_onAccept) m_onAccept(QString());
}

void ShortcutRecorderPanelView::requestClose() { emit closeRequested(); }
