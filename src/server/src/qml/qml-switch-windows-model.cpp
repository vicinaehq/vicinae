#include "qml-switch-windows-model.hpp"
#include "actions/wm/window-actions.hpp"
#include "navigation-controller.hpp"

QString QmlSwitchWindowsModel::displayTitle(const WindowEntry &e) const {
  return e.window->title();
}

QString QmlSwitchWindowsModel::displaySubtitle(const WindowEntry &e) const {
  if (e.app) return e.app->displayName();
  return e.window->wmClass();
}

QString QmlSwitchWindowsModel::displayIconSource(const WindowEntry &e) const {
  if (e.app) return imageSourceFor(e.app->iconUrl());
  return imageSourceFor(ImageURL::builtin("app-window"));
}

QString QmlSwitchWindowsModel::displayAccessory(const WindowEntry &e) const {
  if (auto ws = e.window->workspace()) {
    return QString("WS %1").arg(*ws);
  }
  return {};
}

std::unique_ptr<ActionPanelState>
QmlSwitchWindowsModel::buildActionPanel(const WindowEntry &e) const {
  auto panel = std::make_unique<ListActionPanelState>();

  auto section = panel->createSection("Window Actions");
  section->addAction(new FocusWindowAction(e.window));

  auto closeAction = new CloseWindowAction(e.window);
  closeAction->setShortcut(Keyboard::Shortcut(Qt::Key_Q, Qt::ControlModifier));
  section->addAction(closeAction);

  return panel;
}

QString QmlSwitchWindowsModel::sectionLabel() const {
  return QStringLiteral("Open Windows");
}
