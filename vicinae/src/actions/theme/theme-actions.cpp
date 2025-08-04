#include "theme-actions.hpp"
#include "service-registry.hpp"
#include "services/config/config-service.hpp"

void SetThemeAction::execute(ApplicationContext *ctx) {
  auto configService = ctx->services->config();

  configService->updateConfig([&](ConfigService::Value &value) { value.theme.name = m_themeId; });
  // ui->setToast("Theme successfully updated");
}

SetThemeAction::SetThemeAction(const QString &themeName)
    : AbstractAction("Set theme", ImageURL::builtin("brush")), m_themeId(themeName) {}
