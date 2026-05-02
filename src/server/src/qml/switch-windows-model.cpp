#include "switch-windows-model.hpp"
#include "actions/wm/window-actions.hpp"
#include "navigation-controller.hpp"

QString SwitchWindowsSection::displayTitle(const WindowEntry &e) const { return e.window->title(); }

QString SwitchWindowsSection::displaySubtitle(const WindowEntry &e) const {
  if (e.app) return e.app->displayName();
  return e.window->wmClass();
}

QString SwitchWindowsSection::displayIconSource(const WindowEntry &e) const {
  if (e.app) return imageSourceFor(e.app->iconUrl());
  return imageSourceFor(ImageURL::builtin("app-window"));
}

QVariantList SwitchWindowsSection::displayAccessories(const WindowEntry &e) const {
  if (auto ws = e.window->workspace()) { return qml::textAccessory(QString("WS %1").arg(*ws)); }
  return {};
}

std::unique_ptr<ActionPanelState> SwitchWindowsSection::buildActionPanel(const WindowEntry &e) const {
  auto panel = std::make_unique<ListActionPanelState>();

  auto section = panel->createSection("Window Actions");
  section->addAction(new FocusWindowAction(e.window));

  auto closeAction = new CloseWindowAction(e.window);
  closeAction->setShortcut(Keyboard::Shortcut(Qt::Key_Q, Qt::ControlModifier));
  section->addAction(closeAction);

  return panel;
}
