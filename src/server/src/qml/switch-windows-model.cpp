#include "switch-windows-model.hpp"
#include "actions/app/app-actions.hpp"
#include "actions/wm/window-actions.hpp"

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
  if (!e.workspaceName.isEmpty()) { return qml::textAccessory(e.workspaceName); }
  if (auto ws = e.window->workspace()) { return qml::textAccessory(tr("WS %1").arg(*ws)); }
  return {};
}

std::unique_ptr<ActionPanelState> SwitchWindowsSection::buildActionPanel(const WindowEntry &e) const {
  auto panel = std::make_unique<ListActionPanelState>();

  auto section = panel->createSection(tr("Window Actions"));
  section->addAction(new FocusWindowAction(e.window));

  auto provider = scope().services()->windowManager()->provider();

  if (provider->supportsSetSticky()) { section->addAction(new PinWindowAction(e.window)); }
  if (provider->supportsMoveToWorkspace()) { section->addAction(new BringToWorkspaceAction(e.window)); }

  auto closeAction = new CloseWindowAction(e.window);
  closeAction->setShortcut(Keyboard::Shortcut(Qt::Key_Q, Qt::ControlModifier));
  section->addAction(closeAction);

  if (e.app) {
    auto appSection = panel->createSection();
    appSection->addAction(new QuitAppAction(e.app));
    appSection->addAction(new ForceQuitAppAction(e.app));
  }

  return panel;
}
