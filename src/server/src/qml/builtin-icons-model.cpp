#include "builtin-icons-model.hpp"
#include "clipboard-actions.hpp"
#include "keyboard/keybind.hpp"

QString BuiltinIconsModel::displayTitle(const IconEntry &e) const {
  return QString::fromStdString(e.name);
}

QString BuiltinIconsModel::displayIconSource(const IconEntry &e) const {
  return imageSourceFor(ImageURL::builtin(QString::fromStdString(e.name)));
}

std::unique_ptr<ActionPanelState> BuiltinIconsModel::buildActionPanel(const IconEntry &e) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();

  auto *copyName = new CopyToClipboardAction(Clipboard::Text(e.name.c_str()), "Copy Icon Name");
  copyName->setShortcut(Keybind::CopyNameAction);
  section->addAction(copyName);

  return panel;
}

QString BuiltinIconsModel::sectionLabel() const {
  return QStringLiteral("Icons ({count})");
}
