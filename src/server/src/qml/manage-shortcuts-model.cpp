#include "manage-shortcuts-model.hpp"
#include <QGuiApplication>
#include "actions/shortcut/shortcut-actions.hpp"
#include "keyboard/keybind.hpp"

QString ManageShortcutsModel::displayTitle(const std::shared_ptr<Shortcut> &item) const {
  return item->name();
}

QString ManageShortcutsModel::displayIconSource(const std::shared_ptr<Shortcut> &item) const {
  return imageSourceFor(item->icon());
}

std::unique_ptr<ActionPanelState>
ManageShortcutsModel::buildActionPanel(const std::shared_ptr<Shortcut> &item) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto itemSection = panel->createSection();
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
  mainSection->addAction(edit);
  mainSection->addAction(duplicate);
  dangerSection->addAction(remove);

  return panel;
}

void ManageShortcutsModel::itemSelected(const std::shared_ptr<Shortcut> &item) {
  emit shortcutSelected(item);
}

QString ManageShortcutsModel::sectionLabel() const {
  return QStringLiteral("Shortcuts ({count})");
}
