#include "theme-actions.hpp"
#include "service-registry.hpp"
#include "services/config/config-service.hpp"
#include "services/toast/toast-service.hpp"

void SetThemeAction::execute(ApplicationContext *ctx) {
  auto configService = ctx->services->config();
  auto toast = ctx->services->toastService();

  configService->updateConfig([&](ConfigService::Value &value) { value.theme.name = m_themeId; });
  toast->success("Theme successfully updated");
}

SetThemeAction::SetThemeAction(const QString &themeName)
    : AbstractAction("Set theme", ImageURL::builtin("brush")), m_themeId(themeName) {}
