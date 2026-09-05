#include "windows-notification-client.hpp"
#include "vicinae.hpp"
#include <QDebug>
#include <QDir>
#include <QImage>
#include <QSettings>
#include <QUrl>
#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <winrt/Windows.UI.Notifications.h>

using namespace winrt::Windows::UI::Notifications;
using winrt::Windows::Data::Xml::Dom::XmlDocument;

namespace {
constexpr wchar_t ICON_RESOURCE_NAME[] = L"IDI_ICON1";
constexpr int ICON_SIZE = 256;
const QString REGISTRY_KEY =
    R"(HKEY_CURRENT_USER\Software\Classes\AppUserModelId\)" + Omnicast::APP_USER_MODEL_ID;
const QString ACTIVATION_URL = Omnicast::APP_SCHEME + "://open";

QString appIconPath() { return QString::fromStdString((Omnicast::dataDir() / "app-icon.png").string()); }

bool writeAppIcon(const QString &path) {
  auto icon = static_cast<HICON>(LoadImageW(GetModuleHandleW(nullptr), ICON_RESOURCE_NAME, IMAGE_ICON,
                                            ICON_SIZE, ICON_SIZE, LR_DEFAULTCOLOR));
  if (!icon) return false;

  const QImage image = QImage::fromHICON(icon);
  DestroyIcon(icon);

  return !image.isNull() && image.save(path, "PNG");
}

winrt::hstring toHString(const QString &s) {
  return winrt::hstring(reinterpret_cast<const wchar_t *>(s.utf16()), static_cast<uint32_t>(s.size()));
}

QString toastXml(const AbstractDesktopNotificationClient::Notification &n) {
  using Urgency = AbstractDesktopNotificationClient::Urgency;

  QString image;
  if (n.iconPath) {
    image = QStringLiteral(R"(<image placement="appLogoOverride" src="%1"/>)")
                .arg(QUrl::fromLocalFile(*n.iconPath).toString().toHtmlEscaped());
  }

  const QString audio = n.urgency == Urgency::Low ? QStringLiteral(R"(<audio silent="true"/>)") : QString();

  return QStringLiteral(
             R"(<toast activationType="protocol" launch="%1">)"
             R"(<visual><binding template="ToastGeneric">%2<text>%3</text><text>%4</text></binding></visual>)"
             "%5</toast>")
      .arg(ACTIVATION_URL.toHtmlEscaped(), image, n.title.toHtmlEscaped(), n.body.toHtmlEscaped(), audio);
}
} // namespace

void WindowsNotificationClient::registerAppIdentity() {
  const QString iconPath = appIconPath();
  if (!writeAppIcon(iconPath)) qWarning() << "notification: failed to export app icon to" << iconPath;

  QSettings reg(REGISTRY_KEY, QSettings::NativeFormat);
  reg.setValue("DisplayName", "Vicinae");
  reg.setValue("IconUri", QDir::toNativeSeparators(iconPath));
}

bool WindowsNotificationClient::send(const Notification &n) {
  try {
    XmlDocument doc;
    doc.LoadXml(toHString(toastXml(n)));

    ToastNotification toast(doc);
    if (n.urgency == Urgency::High) toast.Priority(ToastNotificationPriority::High);

    ToastNotificationManager::CreateToastNotifier(toHString(Omnicast::APP_USER_MODEL_ID)).Show(toast);
    return true;
  } catch (const winrt::hresult_error &e) {
    qWarning() << "notification: failed to show toast:" << QString::fromWCharArray(e.message().c_str());
    return false;
  }
}
