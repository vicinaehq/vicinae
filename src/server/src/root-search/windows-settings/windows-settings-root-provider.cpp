#include "root-search/windows-settings/windows-settings-root-provider.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/image/url.hpp"

#include <QFontDatabase>

#include <windows.h>
#include <shellapi.h>

#include <array>

namespace {

constexpr const char *SETTINGS_APP_PARSING_NAME =
    "shell:AppsFolder\\windows.immersivecontrolpanel_cw5n1h2txyewy!microsoft.windows.immersivecontrolpanel";
constexpr const char *SEGOE_FLUENT_FONT = "Segoe Fluent Icons";
constexpr const char *SEGOE_MDL2_FONT = "Segoe MDL2 Assets"; // Windows 10 fallback

const QString &settingsGlyphFont() {
  static const QString family = QFontDatabase::hasFamily(SEGOE_FLUENT_FONT)
                                    ? QString::fromUtf8(SEGOE_FLUENT_FONT)
                                    : QString::fromUtf8(SEGOE_MDL2_FONT);
  return family;
}

// clang-format off
constexpr auto SETTINGS_PAGES = std::to_array<WinSettingsPage>({
    {"display", QT_TRANSLATE_NOOP("WinSettingsPage", "Display"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:display", SegoeIcon::TVMonitor, "monitor resolution brightness scale hdr screen"},
    {"night-light", QT_TRANSLATE_NOOP("WinSettingsPage", "Night Light"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:nightlight", SegoeIcon::QuietHours, "blue light filter"},
    {"sound", QT_TRANSLATE_NOOP("WinSettingsPage", "Sound"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:sound", SegoeIcon::Volume, "audio output input volume speaker microphone"},
    {"volume-mixer", QT_TRANSLATE_NOOP("WinSettingsPage", "Volume Mixer"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:apps-volume", SegoeIcon::Equalizer, "audio per app"},
    {"notifications", QT_TRANSLATE_NOOP("WinSettingsPage", "Notifications"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:notifications", SegoeIcon::Ringer, "banners alerts"},
    {"focus", QT_TRANSLATE_NOOP("WinSettingsPage", "Focus"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:quiethours", SegoeIcon::RingerSilent, "do not disturb dnd quiet hours"},
    {"power", QT_TRANSLATE_NOOP("WinSettingsPage", "Power & Battery"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:powersleep", SegoeIcon::Battery10, "sleep saver screen timeout energy hibernate"},
    {"storage", QT_TRANSLATE_NOOP("WinSettingsPage", "Storage"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:storagesense", SegoeIcon::HardDrive, "disk space cleanup sense"},
    {"nearby-sharing", QT_TRANSLATE_NOOP("WinSettingsPage", "Nearby Sharing"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:crossdevice", SegoeIcon::Share, "share files"},
    {"multitasking", QT_TRANSLATE_NOOP("WinSettingsPage", "Multitasking"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:multitasking", SegoeIcon::TaskView, "snap layouts alt tab virtual desktops"},
    {"activation", QT_TRANSLATE_NOOP("WinSettingsPage", "Activation"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:activation", SegoeIcon::Permissions, "license product key"},
    {"troubleshoot", QT_TRANSLATE_NOOP("WinSettingsPage", "Troubleshoot"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:troubleshoot", SegoeIcon::Repair, "fix problems"},
    {"recovery", QT_TRANSLATE_NOOP("WinSettingsPage", "Recovery"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:recovery", SegoeIcon::UpdateRestore, "reset pc restore reinstall"},
    {"projection", QT_TRANSLATE_NOOP("WinSettingsPage", "Projecting to This PC"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:project", SegoeIcon::MiracastLogoSmall, "miracast wireless display cast"},
    {"remote-desktop", QT_TRANSLATE_NOOP("WinSettingsPage", "Remote Desktop"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:remotedesktop", SegoeIcon::Remote, "rdp"},
    {"clipboard", QT_TRANSLATE_NOOP("WinSettingsPage", "Clipboard"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:clipboard", SegoeIcon::Paste, "history sync paste"},
    {"about", QT_TRANSLATE_NOOP("WinSettingsPage", "About"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:about", SegoeIcon::Info, "system info version specs rename pc"},
    {"optional-features", QT_TRANSLATE_NOOP("WinSettingsPage", "Optional Features"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:optionalfeatures", SegoeIcon::Add, "add windows features"},
    {"developers", QT_TRANSLATE_NOOP("WinSettingsPage", "For Developers"), QT_TRANSLATE_NOOP("WinSettingsPage", "System"), "ms-settings:developers", SegoeIcon::DeveloperTools, "developer mode sudo"},

    {"bluetooth", QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:bluetooth", SegoeIcon::Bluetooth, "pair pairing wireless"},
    {"connected-devices", QT_TRANSLATE_NOOP("WinSettingsPage", "Devices"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:connecteddevices", SegoeIcon::Devices, "add device"},
    {"printers", QT_TRANSLATE_NOOP("WinSettingsPage", "Printers & Scanners"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:printers", SegoeIcon::Print, "print scan"},
    {"mobile-devices", QT_TRANSLATE_NOOP("WinSettingsPage", "Mobile Devices"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:mobile-devices", SegoeIcon::CellPhone, "phone link android iphone"},
    {"cameras", QT_TRANSLATE_NOOP("WinSettingsPage", "Cameras"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:camera", SegoeIcon::Camera, "webcam"},
    {"mouse", QT_TRANSLATE_NOOP("WinSettingsPage", "Mouse"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:mousetouchpad", SegoeIcon::Mouse, "cursor pointer speed scroll"},
    {"touchpad", QT_TRANSLATE_NOOP("WinSettingsPage", "Touchpad"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:devices-touchpad", SegoeIcon::Touchpad, "gestures trackpad"},
    {"pen", QT_TRANSLATE_NOOP("WinSettingsPage", "Pen & Windows Ink"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:pen", SegoeIcon::Edit, "tablet stylus"},
    {"autoplay", QT_TRANSLATE_NOOP("WinSettingsPage", "AutoPlay"), QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:autoplay", SegoeIcon::Play, "removable drive memory card"},
    {"usb", "USB", QT_TRANSLATE_NOOP("WinSettingsPage", "Bluetooth & Devices"), "ms-settings:usb", SegoeIcon::USB, ""},

    {"network", QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-status", SegoeIcon::Globe, "internet status connection"},
    {"wifi", QT_TRANSLATE_NOOP("WinSettingsPage", "Wi-Fi"), QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-wifi", SegoeIcon::Wifi, "wireless wlan"},
    {"ethernet", QT_TRANSLATE_NOOP("WinSettingsPage", "Ethernet"), QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-ethernet", SegoeIcon::Ethernet, "lan wired"},
    {"vpn", "VPN", QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-vpn", SegoeIcon::VPN, "tunnel"},
    {"mobile-hotspot", QT_TRANSLATE_NOOP("WinSettingsPage", "Mobile Hotspot"), QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-mobilehotspot", SegoeIcon::InternetSharing, "tethering"},
    {"airplane-mode", QT_TRANSLATE_NOOP("WinSettingsPage", "Airplane Mode"), QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-airplanemode", SegoeIcon::Airplane, "flight"},
    {"proxy", QT_TRANSLATE_NOOP("WinSettingsPage", "Proxy"), QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-proxy", SegoeIcon::Globe, ""},
    {"dial-up", QT_TRANSLATE_NOOP("WinSettingsPage", "Dial-up"), QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-dialup", SegoeIcon::Phone, "modem"},
    {"advanced-network", QT_TRANSLATE_NOOP("WinSettingsPage", "Advanced Network Settings"), QT_TRANSLATE_NOOP("WinSettingsPage", "Network & Internet"), "ms-settings:network-advancedsettings", SegoeIcon::NetworkAdapter, "adapters data usage"},

    {"background", QT_TRANSLATE_NOOP("WinSettingsPage", "Background"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:personalization-background", SegoeIcon::Picture, "wallpaper desktop"},
    {"colors", QT_TRANSLATE_NOOP("WinSettingsPage", "Colors"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:personalization-colors", SegoeIcon::Color, "accent dark mode light mode theme"},
    {"themes", QT_TRANSLATE_NOOP("WinSettingsPage", "Themes"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:themes", SegoeIcon::Personalize, ""},
    {"lock-screen", QT_TRANSLATE_NOOP("WinSettingsPage", "Lock Screen"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:lockscreen", SegoeIcon::Lock, ""},
    {"touch-keyboard", QT_TRANSLATE_NOOP("WinSettingsPage", "Touch Keyboard"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:personalization-touchkeyboard", SegoeIcon::KeyboardClassic, "osk"},
    {"start", QT_TRANSLATE_NOOP("WinSettingsPage", "Start"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:personalization-start", SegoeIcon::GridView, "start menu"},
    {"taskbar", QT_TRANSLATE_NOOP("WinSettingsPage", "Taskbar"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:taskbar", SegoeIcon::DockBottom, "system tray"},
    {"fonts", QT_TRANSLATE_NOOP("WinSettingsPage", "Fonts"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:fonts", SegoeIcon::Font, "typeface install font"},
    {"dynamic-lighting", QT_TRANSLATE_NOOP("WinSettingsPage", "Dynamic Lighting"), QT_TRANSLATE_NOOP("WinSettingsPage", "Personalization"), "ms-settings:personalization-lighting", SegoeIcon::Lightbulb, "rgb"},

    {"installed-apps", QT_TRANSLATE_NOOP("WinSettingsPage", "Installed Apps"), QT_TRANSLATE_NOOP("WinSettingsPage", "Apps"), "ms-settings:appsfeatures", SegoeIcon::AllApps, "uninstall programs remove add"},
    {"default-apps", QT_TRANSLATE_NOOP("WinSettingsPage", "Default Apps"), QT_TRANSLATE_NOOP("WinSettingsPage", "Apps"), "ms-settings:defaultapps", SegoeIcon::OpenWith, "browser file associations"},
    {"offline-maps", QT_TRANSLATE_NOOP("WinSettingsPage", "Offline Maps"), QT_TRANSLATE_NOOP("WinSettingsPage", "Apps"), "ms-settings:maps", SegoeIcon::DownloadMap, ""},
    {"apps-for-websites", QT_TRANSLATE_NOOP("WinSettingsPage", "Apps for Websites"), QT_TRANSLATE_NOOP("WinSettingsPage", "Apps"), "ms-settings:appsforwebsites", SegoeIcon::Link, ""},
    {"video-playback", QT_TRANSLATE_NOOP("WinSettingsPage", "Video Playback"), QT_TRANSLATE_NOOP("WinSettingsPage", "Apps"), "ms-settings:videoplayback", SegoeIcon::Video, "hdr streaming"},
    {"startup-apps", QT_TRANSLATE_NOOP("WinSettingsPage", "Startup Apps"), QT_TRANSLATE_NOOP("WinSettingsPage", "Apps"), "ms-settings:startupapps", SegoeIcon::LightningBolt, "boot autostart"},

    {"your-info", QT_TRANSLATE_NOOP("WinSettingsPage", "Your Info"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accounts"), "ms-settings:yourinfo", SegoeIcon::Contact, "account picture profile"},
    {"email-accounts", QT_TRANSLATE_NOOP("WinSettingsPage", "Email & Accounts"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accounts"), "ms-settings:emailandaccounts", SegoeIcon::Mail, "mail"},
    {"sign-in-options", QT_TRANSLATE_NOOP("WinSettingsPage", "Sign-in Options"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accounts"), "ms-settings:signinoptions", SegoeIcon::Fingerprint, "windows hello pin password face"},
    {"work-school", QT_TRANSLATE_NOOP("WinSettingsPage", "Access Work or School"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accounts"), "ms-settings:workplace", SegoeIcon::Work, "domain organization"},
    {"family-users", QT_TRANSLATE_NOOP("WinSettingsPage", "Family & Other Users"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accounts"), "ms-settings:otherusers", SegoeIcon::People, "accounts kiosk"},
    {"windows-backup", QT_TRANSLATE_NOOP("WinSettingsPage", "Windows Backup"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accounts"), "ms-settings:backup", SegoeIcon::Cloud, "onedrive sync"},

    {"date-time", QT_TRANSLATE_NOOP("WinSettingsPage", "Date & Time"), QT_TRANSLATE_NOOP("WinSettingsPage", "Time & Language"), "ms-settings:dateandtime", SegoeIcon::Recent, "timezone clock"},
    {"language-region", QT_TRANSLATE_NOOP("WinSettingsPage", "Language & Region"), QT_TRANSLATE_NOOP("WinSettingsPage", "Time & Language"), "ms-settings:regionlanguage", SegoeIcon::TimeLanguage, "locale format keyboard layout"},
    {"typing", QT_TRANSLATE_NOOP("WinSettingsPage", "Typing"), QT_TRANSLATE_NOOP("WinSettingsPage", "Time & Language"), "ms-settings:typing", SegoeIcon::KeyboardClassic, "autocorrect text suggestions"},
    {"speech", QT_TRANSLATE_NOOP("WinSettingsPage", "Speech"), QT_TRANSLATE_NOOP("WinSettingsPage", "Time & Language"), "ms-settings:speech", SegoeIcon::Microphone, "voice"},

    {"game-bar", QT_TRANSLATE_NOOP("WinSettingsPage", "Game Bar"), QT_TRANSLATE_NOOP("WinSettingsPage", "Gaming"), "ms-settings:gaming-gamebar", SegoeIcon::Game, "xbox"},
    {"captures", QT_TRANSLATE_NOOP("WinSettingsPage", "Captures"), QT_TRANSLATE_NOOP("WinSettingsPage", "Gaming"), "ms-settings:gaming-gamedvr", SegoeIcon::Video, "recording screenshots dvr"},
    {"game-mode", QT_TRANSLATE_NOOP("WinSettingsPage", "Game Mode"), QT_TRANSLATE_NOOP("WinSettingsPage", "Gaming"), "ms-settings:gaming-gamemode", SegoeIcon::SpeedHigh, "performance"},

    {"text-size", QT_TRANSLATE_NOOP("WinSettingsPage", "Text Size"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-display", SegoeIcon::FontSize, "bigger accessibility"},
    {"visual-effects", QT_TRANSLATE_NOOP("WinSettingsPage", "Visual Effects"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-visualeffects", SegoeIcon::RedEye, "animations transparency"},
    {"magnifier", QT_TRANSLATE_NOOP("WinSettingsPage", "Magnifier"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-magnifier", SegoeIcon::Zoom, "zoom"},
    {"color-filters", QT_TRANSLATE_NOOP("WinSettingsPage", "Color Filters"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-colorfilter", SegoeIcon::Color, "colorblind"},
    {"contrast-themes", QT_TRANSLATE_NOOP("WinSettingsPage", "Contrast Themes"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-highcontrast", SegoeIcon::Color, "high contrast"},
    {"narrator", QT_TRANSLATE_NOOP("WinSettingsPage", "Narrator"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-narrator", SegoeIcon::Narrator, "screen reader"},
    {"accessibility-audio", QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility Audio"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-audio", SegoeIcon::Headphone, "mono"},
    {"captions", QT_TRANSLATE_NOOP("WinSettingsPage", "Captions"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-closedcaptioning", SegoeIcon::CC, "subtitles"},
    {"accessibility-keyboard", QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility Keyboard"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-keyboard", SegoeIcon::KeyboardClassic, "sticky keys filter keys"},
    {"accessibility-mouse", QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility Mouse"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-mouse", SegoeIcon::Mouse, "mouse keys"},
    {"eye-control", QT_TRANSLATE_NOOP("WinSettingsPage", "Eye Control"), QT_TRANSLATE_NOOP("WinSettingsPage", "Accessibility"), "ms-settings:easeofaccess-eyecontrol", SegoeIcon::RedEye, "tracking"},

    {"windows-security", QT_TRANSLATE_NOOP("WinSettingsPage", "Windows Security"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:windowsdefender", SegoeIcon::Shield, "defender antivirus firewall virus"},
    {"find-my-device", QT_TRANSLATE_NOOP("WinSettingsPage", "Find My Device"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:findmydevice", SegoeIcon::Location, "locate"},
    {"privacy", QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:privacy", SegoeIcon::Lock, "permissions"},
    {"location", QT_TRANSLATE_NOOP("WinSettingsPage", "Location"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:privacy-location", SegoeIcon::MapPin, "gps permissions"},
    {"camera-access", QT_TRANSLATE_NOOP("WinSettingsPage", "Camera Access"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:privacy-webcam", SegoeIcon::Camera, "permissions"},
    {"microphone-access", QT_TRANSLATE_NOOP("WinSettingsPage", "Microphone Access"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:privacy-microphone", SegoeIcon::Microphone, "permissions"},
    {"activity-history", QT_TRANSLATE_NOOP("WinSettingsPage", "Activity History"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:privacy-activityhistory", SegoeIcon::History, ""},
    {"diagnostics", QT_TRANSLATE_NOOP("WinSettingsPage", "Diagnostics & Feedback"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:privacy-feedback", SegoeIcon::Diagnostic, "telemetry"},
    {"search-permissions", QT_TRANSLATE_NOOP("WinSettingsPage", "Search Permissions"), QT_TRANSLATE_NOOP("WinSettingsPage", "Privacy & Security"), "ms-settings:search-permissions", SegoeIcon::Search, "safesearch"},

    {"windows-update", QT_TRANSLATE_NOOP("WinSettingsPage", "Windows Update"), QT_TRANSLATE_NOOP("WinSettingsPage", "Windows Update"), "ms-settings:windowsupdate", SegoeIcon::Sync, "check updates upgrade patch"},
    {"update-history", QT_TRANSLATE_NOOP("WinSettingsPage", "Update History"), QT_TRANSLATE_NOOP("WinSettingsPage", "Windows Update"), "ms-settings:windowsupdate-history", SegoeIcon::History, "installed updates"},
    {"update-advanced", QT_TRANSLATE_NOOP("WinSettingsPage", "Advanced Update Options"), QT_TRANSLATE_NOOP("WinSettingsPage", "Windows Update"), "ms-settings:windowsupdate-options", SegoeIcon::Settings, "active hours delivery optimization"},
    {"windows-insider", QT_TRANSLATE_NOOP("WinSettingsPage", "Windows Insider Program"), QT_TRANSLATE_NOOP("WinSettingsPage", "Windows Update"), "ms-settings:windowsinsider", SegoeIcon::FavoriteStar, "beta dev canary preview"},
});
// clang-format on

class OpenWindowsSettingAction : public AbstractAction {
public:
  OpenWindowsSettingAction(const QString &title, const ImageURL &icon, QString target)
      : AbstractAction(title, icon), m_target(std::move(target)) {}

  void execute(ApplicationContext *ctx) override {
    const std::wstring target = m_target.toStdWString();
    const HINSTANCE ret = ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

    if (reinterpret_cast<INT_PTR>(ret) <= 32) {
      ctx->services->toastService()->failure(
          QCoreApplication::translate("OpenWindowsSettingAction", "Failed to open settings"));
      return;
    }

    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }

private:
  QString m_target;
};

} // namespace

QString WinSettingsPageRootItem::title() const {
  return QCoreApplication::translate("WinSettingsPage", m_page.title);
}

QString WinSettingsPageRootItem::subtitle() const {
  return QCoreApplication::translate("WinSettingsPage", m_page.category);
}

QString WinSettingsPageRootItem::typeDisplayName() const { return tr("System Settings"); }

ImageURL WinSettingsPageRootItem::iconUrl() const {
  return ImageURL::fontPreview(settingsGlyphFont(), QString(QChar(static_cast<char16_t>(m_page.glyph))))
      .setFill(SemanticColor::Foreground);
}

EntrypointId WinSettingsPageRootItem::uniqueId() const { return EntrypointId("windows-settings", m_page.id); }

AccessoryList WinSettingsPageRootItem::accessories() const {
  return {{.text = tr("Settings"), .color = SemanticColor::TextMuted}};
}

std::vector<QString> WinSettingsPageRootItem::keywords() const {
  std::vector<QString> words;
  for (const QString &word : QString::fromUtf8(m_page.keywords).split(' ', Qt::SkipEmptyParts)) {
    words.emplace_back(word);
  }
  return words;
}

std::vector<std::pair<QString, QString>> WinSettingsPageRootItem::settingsMetadata() const {
  return {{tr("Name"), title()}, {tr("Category"), subtitle()}, {QStringLiteral("URL"), m_page.url}};
}

std::unique_ptr<ActionPanelState>
WinSettingsPageRootItem::newActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto utils = panel->createSection();
  auto itemSection = panel->createSection();

  auto open = new OpenWindowsSettingAction(tr("Open %1 Settings").arg(title()), iconUrl(),
                                           QString::fromUtf8(m_page.url));
  mainSection->addAction(open);

  utils->addAction(new CopyToClipboardAction(Clipboard::Text(m_page.url), tr("Copy URL")));

  for (const auto &action : RootSearchActionGenerator::generateActions(*this, metadata)) {
    itemSection->addAction(action);
  }

  panel->setTitle(m_page.title);
  return panel;
}

QString WinSettingsRootProvider::uniqueId() const { return "windows-settings"; }

QString WinSettingsRootProvider::displayName() const { return tr("Windows Settings"); }

QString WinSettingsRootProvider::description() const { return tr("Pages of the Windows Settings app."); }

ImageURL WinSettingsRootProvider::icon() const { return ImageURL::winShellIcon(SETTINGS_APP_PARSING_NAME); }

RootProvider::Type WinSettingsRootProvider::type() const { return RootProvider::Type::GroupProvider; }

std::vector<std::shared_ptr<RootItem>> WinSettingsRootProvider::loadItems() const {
  std::vector<std::shared_ptr<RootItem>> items;
  items.reserve(SETTINGS_PAGES.size());

  for (const auto &page : SETTINGS_PAGES) {
    items.emplace_back(std::make_shared<WinSettingsPageRootItem>(page));
  }

  return items;
}
