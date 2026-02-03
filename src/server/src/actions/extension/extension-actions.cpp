#include "extension-actions.hpp"
#include "ui/alert/alert.hpp"
#include "service-registry.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "navigation-controller.hpp"

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
      ctx->services->toastService()->setToast("Failed to uninstall extension", ToastStyle::Danger);
    }
  });

  ctx->navigation->setDialog(alert);
}
