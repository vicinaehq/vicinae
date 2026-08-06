#include "win-url-scheme-registrar.hpp"
#include "vicinae.hpp"
#include <common/common.hpp>
#include <QSettings>
#include <QDebug>
#include <format>

namespace vicinae::win {

void registerUrlSchemes() {
  const auto handler = findHelperProgram("vicinae-url-handler");

  if (!handler) {
    qWarning() << "url-scheme: vicinae-url-handler binary not found, skipping registration";
    return;
  }

  const QString command = QString::fromStdString(std::format(R"("{}" "%1")", handler->string()));
  const QString icon = QString::fromStdString(std::format("{},0", selfPath().string()));

  for (const QString &scheme : Omnicast::APP_SCHEMES) {
    QSettings reg(R"(HKEY_CURRENT_USER\Software\Classes\)" + scheme, QSettings::NativeFormat);
    reg.setValue("Default", "URL:" + scheme);
    reg.setValue("URL Protocol", "");
    reg.setValue("DefaultIcon/Default", icon);
    reg.setValue("shell/open/command/Default", command);
  }
}

} // namespace vicinae::win
