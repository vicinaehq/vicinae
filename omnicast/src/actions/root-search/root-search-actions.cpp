#include "actions/root-search/root-search-actions.hpp"
#include "omni-icon.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/alert.hpp"
#include "ui/toast.hpp"

void ResetItemRanking::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  auto id = m_id;

  auto callback = [ui, id](bool confirmed) {
    if (!confirmed) return;

    auto manager = ServiceRegistry::instance()->rootItemManager();
    if (manager->resetRanking(id)) {
      ui->setToast("Ranking was successfuly reset");
    } else {
      ui->setToast("Unable to reset ranking");
    }
  };

  auto alert = new CallbackAlertWidget();

  alert->setTitle("Are you sure?");
  alert->setMessage(
      "You will have to rebuild search history for this item in order for it to reappear on top of the "
      "root search results.");
  alert->setConfirmText("Reset", ColorTint::Red);
  alert->setCallback(callback);
  ui->setAlert(alert);
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

OmniIconUrl ToggleItemAsFavorite::icon() const {
  if (m_value) return BuiltinOmniIconUrl("star-disabled");
  return BuiltinOmniIconUrl("star");
}

QString ToggleItemAsFavorite::title() const {
  if (m_value) return "Remove from favorites";
  return "Add to favorites";
}

void ToggleItemAsFavorite::execute() {
  auto manager = ServiceRegistry::instance()->rootItemManager();
  auto ui = ServiceRegistry::instance()->UI();
  bool targetValue = !m_value;

  if (manager->setItemAsFavorite(m_id, targetValue)) {
    if (targetValue) {
      ui->setToast("Successfuly added to favorites");
    } else {
      ui->setToast("Successfuly removed from favorites");
    }
  } else {
    if (targetValue) {
      ui->setToast("Failed to add to favorites");
    } else {
      ui->setToast("Failed to remove from favorites", ToastPriority::Danger);
    }
  }
};

ToggleItemAsFavorite::ToggleItemAsFavorite(const QString &id, bool currentValue)
    : m_id(id), m_value(currentValue) {}

void DefaultActionWrapper::execute(ApplicationContext *ctx) {
  auto manager = ctx->services->rootItemManager();

  if (manager->registerVisit(m_id)) {
    qDebug() << "Visit registered";
  } else {
    qCritical() << "Failed to register visit";
  }

  m_action->execute(ctx);
}

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
