#include "manage-shortcuts-model.hpp"
#include <QGuiApplication>
#include "actions/shortcut/shortcut-actions.hpp"
#include "keyboard/keybind.hpp"

QString ManageShortcutsSection::displayTitle(const std::shared_ptr<Shortcut> &item) const {
  return item->name();
}

QString ManageShortcutsSection::displayIconSource(const std::shared_ptr<Shortcut> &item) const {
  return imageSourceFor(item->icon());
}

std::unique_ptr<ActionPanelState>
ManageShortcutsSection::buildActionPanel(const std::shared_ptr<Shortcut> &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto manageSection = panel->createSection();
  auto dangerSection = panel->createSection();

  auto open = new OpenCompletedShortcutAction(item);
  auto edit = new EditShortcutAction(item);
  auto duplicate = new DuplicateShortcutAction(item);
  auto remove = new RemoveShortcutAction(item);

  duplicate->setShortcut(Keybind::DuplicateAction);
  edit->setShortcut(Keybind::EditAction);
  remove->setShortcut(Keybind::RemoveAction);

  panel->setTitle(item->name());
  mainSection->addAction(open);
  manageSection->addAction(edit);
  manageSection->addAction(duplicate);
  dangerSection->addAction(remove);

  return panel;
}
