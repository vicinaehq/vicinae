#include "actions/root-search/root-search-actions.hpp"
#include "common.hpp"
#include "../../ui/image/url.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/alert/alert.hpp"
#include "ui/toast/toast.hpp"
#include "navigation-controller.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"

void ResetItemRanking::execute(ApplicationContext *ctx) {
  auto id = m_id;

  auto callback = [ctx, id](bool confirmed) {
    if (!confirmed) return;

    auto toast = ctx->services->toastService();

    auto manager = ServiceRegistry::instance()->rootItemManager();
    if (manager->resetRanking(id)) {
      toast->setToast("Ranking was successfuly reset");
    } else {
      toast->setToast("Unable to reset ranking");
    }
  };

  auto alert = new CallbackAlertWidget();

  alert->setTitle("Are you sure?");
  alert->setMessage(
      "You will have to rebuild search history for this item in order for it to reappear on top of the "
      "root search results.");
  alert->setConfirmText("Reset", SemanticColor::Red);
  alert->setCallback(callback);
  ctx->navigation->setDialog(alert);
}

ResetItemRanking::ResetItemRanking(const QString &id)
    : AbstractAction("Reset ranking", ImageURL::builtin("arrow-counter-clockwise")), m_id(id) {}

void MarkItemAsFavorite::execute(ApplicationContext *ctx) {
  // TODO: mark as favorite
}

MarkItemAsFavorite::MarkItemAsFavorite(const QString &id)
    : AbstractAction("Mark as favorite", ImageURL::builtin("stars")), m_id(id) {}

ImageURL ToggleItemAsFavorite::icon() const {
  if (m_value) return ImageURL::builtin("star-disabled");
  return ImageURL::builtin("star");
}

QString ToggleItemAsFavorite::title() const {
  if (m_value) return "Remove from favorites";
  return "Add to favorites";
}

void ToggleItemAsFavorite::execute(ApplicationContext *ctx) {
  auto manager = ctx->services->rootItemManager();
  auto toast = ctx->services->toastService();
  bool targetValue = !m_value;

  if (manager->setItemAsFavorite(m_id, targetValue)) {
    if (targetValue) {
      toast->setToast("Successfuly added to favorites");
    } else {
      toast->setToast("Successfuly removed from favorites");
    }
  } else {
    if (targetValue) {
      toast->setToast("Failed to add to favorites");
    } else {
      toast->setToast("Failed to remove from favorites", ToastPriority::Danger);
    }
  }
};

ToggleItemAsFavorite::ToggleItemAsFavorite(const QString &id, bool currentValue)
    : m_id(id), m_value(currentValue) {}

void DefaultActionWrapper::execute(ApplicationContext *ctx) {
  auto manager = ctx->services->rootItemManager();

  if (manager->registerVisit(m_id)) {
  } else {
    qWarning() << "Failed to register root item visit";
  }

  m_action->execute(ctx);
  ctx->navigation->clearSearchText();
}

QString DefaultActionWrapper::title() const { return m_action->title(); }

DefaultActionWrapper::DefaultActionWrapper(const QString &id, AbstractAction *action)
    : AbstractAction(action->title(), action->iconUrl), m_id(id), m_action(action) {
  setPrimary(true);
  setShortcut({.key = "return"});
}

void DisableItemAction::execute(ApplicationContext *ctx) {
  auto alert = new CallbackAlertWidget();

  alert->setTitle("Are you sure?");
  alert->setMessage("You will need to go in the settings to manually re-enable it.");
  alert->setConfirmText("Disable", SemanticColor::Red);
  alert->setCallback([ctx, id = m_id](bool ok) {
    auto manager = ctx->services->rootItemManager();
    auto toast = ctx->services->toastService();

    if (!ok) return;

    if (manager->disableItem(id)) {
      toast->setToast("Item disabled", ToastPriority::Success);
    } else {
      toast->setToast("Failed to disable", ToastPriority::Danger);
    }
  });

  ctx->navigation->setDialog(alert);
}

DisableItemAction::DisableItemAction(const QString &id)
    : AbstractAction("Disable item", ImageURL::builtin("trash")), m_id(id) {
  setStyle(AbstractAction::Danger);
}

void UninstallExtensionAction::execute(ApplicationContext *ctx) {
  auto alert = new CallbackAlertWidget();

  alert->setTitle("Are you sure?");
  alert->setMessage(
      "All this extension data will be permanently lost. If you "
      "just want the extension to not appear in the root search anymore, consider disabling it instead.");
  alert->setConfirmText("Uninstall", SemanticColor::Red);
  alert->setCallback([ctx, id = m_id](bool ok) {
    if (!ok) return;
    if (ctx->services->extensionRegistry()->uninstall(id)) {
      ctx->services->toastService()->setToast("Extension uninstalled");
    } else {
      ctx->services->toastService()->setToast("Failed to uninstall extension", ToastPriority::Danger);
    }
  });

  ctx->navigation->setDialog(alert);
}
