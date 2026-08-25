#include "actions/root-search/root-search-actions.hpp"
#include "qml/alias-form-view-host.hpp"
#include "keyboard/keyboard.hpp"
#include "keyboard/keybind.hpp"
#include "ui/action-pannel/shortcut-recorder-panel-view.hpp"
#include "ui/image/url.hpp"
#include "service-registry.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/alert/alert.hpp"
#include "ui/toast/toast.hpp"
#include "navigation-controller.hpp"
#include "services/toast/toast-service.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "settings-controller/settings-controller.hpp"

void ResetItemRanking::execute(ApplicationContext *ctx) {
  auto id = m_id;

  auto callback = [ctx, id](bool confirmed) {
    if (!confirmed) return;

    auto toast = ctx->services->toastService();

    auto manager = ServiceRegistry::instance()->rootItemManager();
    if (manager->resetRanking(id)) {
      toast->setToast(tr("Ranking was successfully reset"));
    } else {
      toast->setToast(tr("Unable to reset ranking"));
    }
  };

  auto alert = new CallbackAlertWidget();

  alert->setTitle(tr("Are you sure?"));
  alert->setMessage(
      tr("You will have to rebuild search history for this item in order for it to reappear on top of the "
         "root search results."));
  alert->setConfirmText(tr("Reset"), SemanticColor::Red);
  alert->setCallback(callback);
  ctx->navigation->setDialog(alert);
}

ResetItemRanking::ResetItemRanking(const EntrypointId &id)
    : AbstractAction(tr("Reset ranking"), ImageURL::builtin(BuiltinIcon::ArrowCounterClockwise)), m_id(id) {}

void MarkItemAsFavorite::execute(ApplicationContext *ctx) {
  // TODO: mark as favorite
}

MarkItemAsFavorite::MarkItemAsFavorite(const QString &id)
    : AbstractAction(QCoreApplication::translate("MarkItemAsFavorite", "Mark as favorite"),
                     ImageURL::builtin(BuiltinIcon::Stars)),
      m_id(id) {}

std::optional<ImageURL> ToggleItemAsFavorite::icon() const {
  if (m_value) return ImageURL::builtin(BuiltinIcon::StarDisabled);
  return ImageURL::builtin(BuiltinIcon::Star);
}

QString ToggleItemAsFavorite::title() const {
  if (m_value) return tr("Remove from favorites");
  return tr("Add to favorites");
}

void ToggleItemAsFavorite::execute(ApplicationContext *ctx) {
  auto manager = ctx->services->rootItemManager();
  auto toast = ctx->services->toastService();
  bool const targetValue = !m_value;

  if (manager->setItemAsFavorite(m_id, targetValue)) {
    if (targetValue) {
      toast->setToast(tr("Successfuly added to favorites"));
    } else {
      toast->setToast(tr("Successfuly removed from favorites"));
    }
  } else {
    if (targetValue) {
      toast->setToast(tr("Failed to add to favorites"));
    } else {
      toast->setToast(tr("Failed to remove from favorites"), ToastStyle::Danger);
    }
  }
};

ToggleItemAsFavorite::ToggleItemAsFavorite(const EntrypointId &id, bool currentValue)
    : m_id(id), m_value(currentValue) {
  setShortcut(Keyboard::Shortcut(Qt::Key_F, Qt::ControlModifier | Qt::ShiftModifier));
}

void MoveFavoriteUpAction::execute(ApplicationContext *ctx) {
  ctx->services->rootItemManager()->moveFavoriteUp(m_id);
}

MoveFavoriteUpAction::MoveFavoriteUpAction(const EntrypointId &id)
    : AbstractAction(tr("Move up in favorites"), ImageURL::builtin(BuiltinIcon::ArrowUp)), m_id(id) {
  setShortcut(Keybind::MoveUpAction);
}

void MoveFavoriteDownAction::execute(ApplicationContext *ctx) {
  ctx->services->rootItemManager()->moveFavoriteDown(m_id);
}

MoveFavoriteDownAction::MoveFavoriteDownAction(const EntrypointId &id)
    : AbstractAction(tr("Move down in favorites"), ImageURL::builtin(BuiltinIcon::ArrowDown)), m_id(id) {
  setShortcut(Keybind::MoveDownAction);
}

void DisableItemAction::execute(ApplicationContext *ctx) {
  auto alert = new CallbackAlertWidget();

  alert->setTitle(tr("Are you sure?"));
  alert->setMessage(tr("You will need to go in the settings to manually re-enable it."));
  alert->setConfirmText(tr("Disable"), SemanticColor::Red);
  alert->setCallback([ctx, id = m_id](bool ok) {
    auto manager = ctx->services->rootItemManager();
    auto toast = ctx->services->toastService();

    if (!ok) return;

    if (manager->disableItem(id)) {
      toast->setToast(tr("Item disabled"), ToastStyle::Success);
    } else {
      toast->setToast(tr("Failed to disable"), ToastStyle::Danger);
    }
  });

  ctx->navigation->setDialog(alert);
}

DisableItemAction::DisableItemAction(const EntrypointId &id)
    : AbstractAction(tr("Disable item"), ImageURL::builtin(BuiltinIcon::Trash)), m_id(id) {
  setStyle(AbstractAction::Style::Danger);
}

void OpenItemPreferencesAction::execute(ApplicationContext *ctx) {
  ctx->navigation->closeWindow();
  ctx->settings->openExtensionPreferences(m_id);
}

void SetRootItemAliasAction::execute(ApplicationContext *ctx) {
  ctx->navigation->pushView(new AliasFormViewHost(m_id));
}

SetRootItemShortcutAction::SetRootItemShortcutAction(const EntrypointId &id, const QString &itemTitle,
                                                     const ImageURL &itemIcon,
                                                     const std::optional<std::string> &shortcut)
    : SubmenuAction(tr("Set Global Shortcut"), ImageURL::builtin(BuiltinIcon::Keyboard)), m_id(id),
      m_itemTitle(itemTitle), m_itemIcon(itemIcon),
      m_shortcut(QString::fromStdString(shortcut.value_or(""))) {}

ActionPanelView *SetRootItemShortcutAction::createView(ApplicationContext *ctx, QObject *parent) {
  auto *view = new ShortcutRecorderPanelView(m_itemTitle, m_itemIcon, QString::fromStdString(m_id),
                                             m_shortcut, parent);

  view->setAcceptHandler([ctx, id = m_id](const QString &serialized) {
    ctx->services->rootItemManager()->setShortcut(id, serialized.toStdString());
  });

  return view;
}
