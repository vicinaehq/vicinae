#include "theme-actions.hpp"
#include "service-registry.hpp"

void SetThemeAction::execute() {
  auto ui = ServiceRegistry::instance()->UI();
  auto configService = ServiceRegistry::instance()->config();

  configService->updateConfig([&](ConfigService::Value &value) { value.theme.name = m_themeId; });

  ThemeService::instance().setTheme(m_themeId);
  ui->setToast("Theme successfully updated");
}

SetThemeAction::SetThemeAction(const QString &themeName)
    : AbstractAction("Set theme", BuiltinOmniIconUrl("brush")), m_themeId(themeName) {}
