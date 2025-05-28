#include "actions/root-search/root-search-actions.hpp"
#include "service-registry.hpp"

void ResetItemRanking::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  auto ui = ServiceRegistry::instance()->UI();

  // TODO: reset ranking
}

ResetItemRanking::ResetItemRanking(const QString &id)
    : AbstractAction("Reset ranking", BuiltinOmniIconUrl("arrow-counter-clockwise")), m_id(id) {}

void MarkItemAsFavorite::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  auto ui = ServiceRegistry::instance()->UI();

  // TODO: mark as favorite
}

MarkItemAsFavorite::MarkItemAsFavorite(const QString &id)
    : AbstractAction("Mark as favorite", BuiltinOmniIconUrl("stars")), m_id(id) {}

void DefaultActionWrapper::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();

  if (manager->registerVisit(m_id)) {
    qDebug() << "Visit registered";
  } else {
    qCritical() << "Failed to register visit";
  }

  m_action->execute();
}

QString DefaultActionWrapper::title() const { return m_action->title(); }

DefaultActionWrapper::DefaultActionWrapper(const QString &id, AbstractAction *action)
    : AbstractAction(action->title(), action->iconUrl), m_id(id), m_action(action) {
  setPrimary(true);
  setShortcut({.key = "return"});
}

void DisableItemAction::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  auto ui = ServiceRegistry::instance()->UI();

  if (manager->disableItem(m_id)) {
    ui->setToast("Item disabled", ToastPriority::Success);
  } else {
    ui->setToast("Failed to disable", ToastPriority::Danger);
  }
}

DisableItemAction::DisableItemAction(const QString &id)
    : AbstractAction("Disable item", BuiltinOmniIconUrl("trash")), m_id(id) {
  setStyle(AbstractAction::Danger);
}
