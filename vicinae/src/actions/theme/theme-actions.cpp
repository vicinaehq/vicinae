#include "theme-actions.hpp"
#include "service-registry.hpp"
#include "config/config.hpp"
#include "services/toast/toast-service.hpp"

void SetThemeAction::execute(ApplicationContext *ctx) {
  auto cfg = ctx->services->config();
  auto toast = ctx->services->toastService();

  cfg->mergeThemeConfig({.name = m_themeId.toStdString()});
  toast->success("Theme successfully updated");
}

SetThemeAction::SetThemeAction(const QString &themeName)
    : AbstractAction("Set theme", ImageURL::builtin("brush")), m_themeId(themeName) {}
