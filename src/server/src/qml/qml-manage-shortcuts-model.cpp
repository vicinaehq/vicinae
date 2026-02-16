#include "qml-manage-shortcuts-model.hpp"
#include <qapplication.h>
#include "actions/shortcut/shortcut-actions.hpp"
#include "keyboard/keybind.hpp"

QString QmlManageShortcutsModel::displayTitle(const std::shared_ptr<Shortcut> &item) const {
  return item->name();
}

QString QmlManageShortcutsModel::displayIconSource(const std::shared_ptr<Shortcut> &item) const {
  return imageSourceFor(item->icon());
}

std::unique_ptr<ActionPanelState>
QmlManageShortcutsModel::buildActionPanel(const std::shared_ptr<Shortcut> &item) const {
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

void QmlManageShortcutsModel::itemSelected(const std::shared_ptr<Shortcut> &item) {
  emit shortcutSelected(item);
}

QString QmlManageShortcutsModel::sectionLabel() const {
  return QStringLiteral("Shortcuts ({count})");
}
