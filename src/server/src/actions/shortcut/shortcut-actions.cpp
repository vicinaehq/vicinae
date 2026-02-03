#include "actions/shortcut/shortcut-actions.hpp"
#include "create-quicklink-command.hpp"
#include "service-registry.hpp"
#include "ui/views/base-view.hpp"

void OpenShortcutAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  auto appDb = ServiceRegistry::instance()->appDb();
  QString expanded;
  size_t argumentIndex = 0;

  for (const auto &part : m_shortcut->parts()) {
    if (auto s = std::get_if<QString>(&part)) {
      expanded += *s;
    } else if (auto placeholder = std::get_if<Shortcut::ParsedPlaceholder>(&part)) {
      if (placeholder->id == "clipboard") {
        expanded += QApplication::clipboard()->text();
      } else if (placeholder->id == "selected") {
        // TODO: selected text
      } else if (placeholder->id == "uuid") {
        expanded += QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
      } else {
        if (argumentIndex < m_arguments.size()) { expanded += m_arguments.at(argumentIndex++); }
      }
    }
  }

  if (auto app = appDb->findById(m_shortcut->app())) { appDb->launch(*app, {expanded}); }

  ui->popToRoot();
  ui->closeWindow();
}

OpenShortcutAction::OpenShortcutAction(const std::shared_ptr<Shortcut> &shortcut,
                                       const std::vector<QString> &arguments)
    : AbstractAction("Open shortcut", shortcut->icon()), m_shortcut(shortcut), m_arguments(arguments) {}

// OpenCompletedShortcutAction

void OpenCompletedShortcutAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  OpenShortcutAction open(m_shortcut, ui->topView()->argumentValues());

  open.execute();
}

OpenCompletedShortcutAction::OpenCompletedShortcutAction(const std::shared_ptr<Shortcut> &shortcut)
    : AbstractAction("Open shortcut", shortcut->icon()), m_shortcut(shortcut) {}

// OpenShortcutFromSearchText

void OpenShortcutFromSearchText::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  OpenShortcutAction open(m_shortcut, {ui->topView()->searchText()});

  open.execute();
}

OpenShortcutFromSearchText::OpenShortcutFromSearchText(const std::shared_ptr<Shortcut> &shortcut)
    : AbstractAction("Open shortcut", shortcut->icon()), m_shortcut(shortcut) {}

// EditShortcutAction

void EditShortcutAction::execute() {
  auto view = new EditShortcutView(m_shortcut);
  auto ui = ServiceRegistry::instance()->UI();

  ui->pushView(view);
}

EditShortcutAction::EditShortcutAction(const std::shared_ptr<Shortcut> &shortcut, const QList<QString> &args)
    : AbstractAction("Edit shortcut", ImageURL::builtin("pencil")), m_shortcut(shortcut) {}

// RemoveShortcutAction

void RemoveShortcutAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  auto shortcutDb = ServiceRegistry::instance()->shortcuts();
  bool removeResult = shortcutDb->removeShortcut(m_shortcut->id());

  if (removeResult) {
    ui->setToast("Removed link");
  } else {
    ui->setToast("Failed to remove link", ToastPriority::Danger);
  }
}

RemoveShortcutAction::RemoveShortcutAction(const std::shared_ptr<Shortcut> &link)
    : AbstractAction("Remove link", ImageURL::builtin("trash")), m_shortcut(link) {}

DuplicateShortcutAction::DuplicateShortcutAction(const std::shared_ptr<Shortcut> &link)
    : AbstractAction("Duplicate link", ImageURL::builtin("duplicate")), link(link) {}
