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
constexpr WinSettingsPage SETTINGS_PAGES[] = {
    {"display", "Display", "System", "ms-settings:display", SegoeIcon::TVMonitor, "monitor resolution brightness scale hdr screen"},
    {"night-light", "Night Light", "System", "ms-settings:nightlight", SegoeIcon::QuietHours, "blue light filter"},
    {"sound", "Sound", "System", "ms-settings:sound", SegoeIcon::Volume, "audio output input volume speaker microphone"},
    {"volume-mixer", "Volume Mixer", "System", "ms-settings:apps-volume", SegoeIcon::Equalizer, "audio per app"},
    {"notifications", "Notifications", "System", "ms-settings:notifications", SegoeIcon::Ringer, "banners alerts"},
    {"focus", "Focus", "System", "ms-settings:quiethours", SegoeIcon::RingerSilent, "do not disturb dnd quiet hours"},
    {"power", "Power & Battery", "System", "ms-settings:powersleep", SegoeIcon::Battery10, "sleep saver screen timeout energy hibernate"},
    {"storage", "Storage", "System", "ms-settings:storagesense", SegoeIcon::HardDrive, "disk space cleanup sense"},
    {"nearby-sharing", "Nearby Sharing", "System", "ms-settings:crossdevice", SegoeIcon::Share, "share files"},
    {"multitasking", "Multitasking", "System", "ms-settings:multitasking", SegoeIcon::TaskView, "snap layouts alt tab virtual desktops"},
    {"activation", "Activation", "System", "ms-settings:activation", SegoeIcon::Permissions, "license product key"},
    {"troubleshoot", "Troubleshoot", "System", "ms-settings:troubleshoot", SegoeIcon::Repair, "fix problems"},
    {"recovery", "Recovery", "System", "ms-settings:recovery", SegoeIcon::UpdateRestore, "reset pc restore reinstall"},
    {"projection", "Projecting to This PC", "System", "ms-settings:project", SegoeIcon::MiracastLogoSmall, "miracast wireless display cast"},
    {"remote-desktop", "Remote Desktop", "System", "ms-settings:remotedesktop", SegoeIcon::Remote, "rdp"},
    {"clipboard", "Clipboard", "System", "ms-settings:clipboard", SegoeIcon::Paste, "history sync paste"},
    {"about", "About", "System", "ms-settings:about", SegoeIcon::Info, "system info version specs rename pc"},
    {"optional-features", "Optional Features", "System", "ms-settings:optionalfeatures", SegoeIcon::Add, "add windows features"},
    {"developers", "For Developers", "System", "ms-settings:developers", SegoeIcon::DeveloperTools, "developer mode sudo"},

    {"bluetooth", "Bluetooth & Devices", "Bluetooth & Devices", "ms-settings:bluetooth", SegoeIcon::Bluetooth, "pair pairing wireless"},
    {"connected-devices", "Devices", "Bluetooth & Devices", "ms-settings:connecteddevices", SegoeIcon::Devices, "add device"},
    {"printers", "Printers & Scanners", "Bluetooth & Devices", "ms-settings:printers", SegoeIcon::Print, "print scan"},
    {"mobile-devices", "Mobile Devices", "Bluetooth & Devices", "ms-settings:mobile-devices", SegoeIcon::CellPhone, "phone link android iphone"},
    {"cameras", "Cameras", "Bluetooth & Devices", "ms-settings:camera", SegoeIcon::Camera, "webcam"},
    {"mouse", "Mouse", "Bluetooth & Devices", "ms-settings:mousetouchpad", SegoeIcon::Mouse, "cursor pointer speed scroll"},
    {"touchpad", "Touchpad", "Bluetooth & Devices", "ms-settings:devices-touchpad", SegoeIcon::Touchpad, "gestures trackpad"},
    {"pen", "Pen & Windows Ink", "Bluetooth & Devices", "ms-settings:pen", SegoeIcon::Edit, "tablet stylus"},
    {"autoplay", "AutoPlay", "Bluetooth & Devices", "ms-settings:autoplay", SegoeIcon::Play, "removable drive memory card"},
    {"usb", "USB", "Bluetooth & Devices", "ms-settings:usb", SegoeIcon::USB, ""},

    {"network", "Network & Internet", "Network & Internet", "ms-settings:network-status", SegoeIcon::Globe, "internet status connection"},
    {"wifi", "Wi-Fi", "Network & Internet", "ms-settings:network-wifi", SegoeIcon::Wifi, "wireless wlan"},
    {"ethernet", "Ethernet", "Network & Internet", "ms-settings:network-ethernet", SegoeIcon::Ethernet, "lan wired"},
    {"vpn", "VPN", "Network & Internet", "ms-settings:network-vpn", SegoeIcon::VPN, "tunnel"},
    {"mobile-hotspot", "Mobile Hotspot", "Network & Internet", "ms-settings:network-mobilehotspot", SegoeIcon::InternetSharing, "tethering"},
    {"airplane-mode", "Airplane Mode", "Network & Internet", "ms-settings:network-airplanemode", SegoeIcon::Airplane, "flight"},
    {"proxy", "Proxy", "Network & Internet", "ms-settings:network-proxy", SegoeIcon::Globe, ""},
    {"dial-up", "Dial-up", "Network & Internet", "ms-settings:network-dialup", SegoeIcon::Phone, "modem"},
    {"advanced-network", "Advanced Network Settings", "Network & Internet", "ms-settings:network-advancedsettings", SegoeIcon::NetworkAdapter, "adapters data usage"},

    {"background", "Background", "Personalization", "ms-settings:personalization-background", SegoeIcon::Picture, "wallpaper desktop"},
    {"colors", "Colors", "Personalization", "ms-settings:personalization-colors", SegoeIcon::Color, "accent dark mode light mode theme"},
    {"themes", "Themes", "Personalization", "ms-settings:themes", SegoeIcon::Personalize, ""},
    {"lock-screen", "Lock Screen", "Personalization", "ms-settings:lockscreen", SegoeIcon::Lock, ""},
    {"touch-keyboard", "Touch Keyboard", "Personalization", "ms-settings:personalization-touchkeyboard", SegoeIcon::KeyboardClassic, "osk"},
    {"start", "Start", "Personalization", "ms-settings:personalization-start", SegoeIcon::GridView, "start menu"},
    {"taskbar", "Taskbar", "Personalization", "ms-settings:taskbar", SegoeIcon::DockBottom, "system tray"},
    {"fonts", "Fonts", "Personalization", "ms-settings:fonts", SegoeIcon::Font, "typeface install font"},
    {"dynamic-lighting", "Dynamic Lighting", "Personalization", "ms-settings:personalization-lighting", SegoeIcon::Lightbulb, "rgb"},

    {"installed-apps", "Installed Apps", "Apps", "ms-settings:appsfeatures", SegoeIcon::AllApps, "uninstall programs remove add"},
    {"default-apps", "Default Apps", "Apps", "ms-settings:defaultapps", SegoeIcon::OpenWith, "browser file associations"},
    {"offline-maps", "Offline Maps", "Apps", "ms-settings:maps", SegoeIcon::DownloadMap, ""},
    {"apps-for-websites", "Apps for Websites", "Apps", "ms-settings:appsforwebsites", SegoeIcon::Link, ""},
    {"video-playback", "Video Playback", "Apps", "ms-settings:videoplayback", SegoeIcon::Video, "hdr streaming"},
    {"startup-apps", "Startup Apps", "Apps", "ms-settings:startupapps", SegoeIcon::LightningBolt, "boot autostart"},

    {"your-info", "Your Info", "Accounts", "ms-settings:yourinfo", SegoeIcon::Contact, "account picture profile"},
    {"email-accounts", "Email & Accounts", "Accounts", "ms-settings:emailandaccounts", SegoeIcon::Mail, "mail"},
    {"sign-in-options", "Sign-in Options", "Accounts", "ms-settings:signinoptions", SegoeIcon::Fingerprint, "windows hello pin password face"},
    {"work-school", "Access Work or School", "Accounts", "ms-settings:workplace", SegoeIcon::Work, "domain organization"},
    {"family-users", "Family & Other Users", "Accounts", "ms-settings:otherusers", SegoeIcon::People, "accounts kiosk"},
    {"windows-backup", "Windows Backup", "Accounts", "ms-settings:backup", SegoeIcon::Cloud, "onedrive sync"},

    {"date-time", "Date & Time", "Time & Language", "ms-settings:dateandtime", SegoeIcon::Recent, "timezone clock"},
    {"language-region", "Language & Region", "Time & Language", "ms-settings:regionlanguage", SegoeIcon::TimeLanguage, "locale format keyboard layout"},
    {"typing", "Typing", "Time & Language", "ms-settings:typing", SegoeIcon::KeyboardClassic, "autocorrect text suggestions"},
    {"speech", "Speech", "Time & Language", "ms-settings:speech", SegoeIcon::Microphone, "voice"},

    {"game-bar", "Game Bar", "Gaming", "ms-settings:gaming-gamebar", SegoeIcon::Game, "xbox"},
    {"captures", "Captures", "Gaming", "ms-settings:gaming-gamedvr", SegoeIcon::Video, "recording screenshots dvr"},
    {"game-mode", "Game Mode", "Gaming", "ms-settings:gaming-gamemode", SegoeIcon::SpeedHigh, "performance"},

    {"text-size", "Text Size", "Accessibility", "ms-settings:easeofaccess-display", SegoeIcon::FontSize, "bigger accessibility"},
    {"visual-effects", "Visual Effects", "Accessibility", "ms-settings:easeofaccess-visualeffects", SegoeIcon::RedEye, "animations transparency"},
    {"magnifier", "Magnifier", "Accessibility", "ms-settings:easeofaccess-magnifier", SegoeIcon::Zoom, "zoom"},
    {"color-filters", "Color Filters", "Accessibility", "ms-settings:easeofaccess-colorfilter", SegoeIcon::Color, "colorblind"},
    {"contrast-themes", "Contrast Themes", "Accessibility", "ms-settings:easeofaccess-highcontrast", SegoeIcon::Color, "high contrast"},
    {"narrator", "Narrator", "Accessibility", "ms-settings:easeofaccess-narrator", SegoeIcon::Narrator, "screen reader"},
    {"accessibility-audio", "Accessibility Audio", "Accessibility", "ms-settings:easeofaccess-audio", SegoeIcon::Headphone, "mono"},
    {"captions", "Captions", "Accessibility", "ms-settings:easeofaccess-closedcaptioning", SegoeIcon::CC, "subtitles"},
    {"accessibility-keyboard", "Accessibility Keyboard", "Accessibility", "ms-settings:easeofaccess-keyboard", SegoeIcon::KeyboardClassic, "sticky keys filter keys"},
    {"accessibility-mouse", "Accessibility Mouse", "Accessibility", "ms-settings:easeofaccess-mouse", SegoeIcon::Mouse, "mouse keys"},
    {"eye-control", "Eye Control", "Accessibility", "ms-settings:easeofaccess-eyecontrol", SegoeIcon::RedEye, "tracking"},

    {"windows-security", "Windows Security", "Privacy & Security", "ms-settings:windowsdefender", SegoeIcon::Shield, "defender antivirus firewall virus"},
    {"find-my-device", "Find My Device", "Privacy & Security", "ms-settings:findmydevice", SegoeIcon::Location, "locate"},
    {"privacy", "Privacy", "Privacy & Security", "ms-settings:privacy", SegoeIcon::Lock, "permissions"},
    {"location", "Location", "Privacy & Security", "ms-settings:privacy-location", SegoeIcon::MapPin, "gps permissions"},
    {"camera-access", "Camera Access", "Privacy & Security", "ms-settings:privacy-webcam", SegoeIcon::Camera, "permissions"},
    {"microphone-access", "Microphone Access", "Privacy & Security", "ms-settings:privacy-microphone", SegoeIcon::Microphone, "permissions"},
    {"activity-history", "Activity History", "Privacy & Security", "ms-settings:privacy-activityhistory", SegoeIcon::History, ""},
    {"diagnostics", "Diagnostics & Feedback", "Privacy & Security", "ms-settings:privacy-feedback", SegoeIcon::Diagnostic, "telemetry"},
    {"search-permissions", "Search Permissions", "Privacy & Security", "ms-settings:search-permissions", SegoeIcon::Search, "safesearch"},

    {"windows-update", "Windows Update", "Windows Update", "ms-settings:windowsupdate", SegoeIcon::Sync, "check updates upgrade patch"},
    {"update-history", "Update History", "Windows Update", "ms-settings:windowsupdate-history", SegoeIcon::History, "installed updates"},
    {"update-advanced", "Advanced Update Options", "Windows Update", "ms-settings:windowsupdate-options", SegoeIcon::Settings, "active hours delivery optimization"},
    {"windows-insider", "Windows Insider Program", "Windows Update", "ms-settings:windowsinsider", SegoeIcon::FavoriteStar, "beta dev canary preview"},
};
// clang-format on

class OpenWindowsSettingAction : public AbstractAction {
public:
  OpenWindowsSettingAction(const QString &title, const ImageURL &icon, QString target)
      : AbstractAction(title, icon), m_target(std::move(target)) {}

  void execute(ApplicationContext *ctx) override {
    const std::wstring target = m_target.toStdWString();
    const HINSTANCE ret = ShellExecuteW(nullptr, L"open", target.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

    if (reinterpret_cast<INT_PTR>(ret) <= 32) {
      ctx->services->toastService()->failure("Failed to open settings");
      return;
    }

    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }

private:
  QString m_target;
};

} // namespace

QString WinSettingsPageRootItem::title() const { return m_page.title; }

QString WinSettingsPageRootItem::subtitle() const { return m_page.category; }

QString WinSettingsPageRootItem::typeDisplayName() const { return "System Settings"; }

ImageURL WinSettingsPageRootItem::iconUrl() const {
  return ImageURL::fontPreview(settingsGlyphFont(), QString(QChar(static_cast<char16_t>(m_page.glyph))))
      .setFill(SemanticColor::Foreground);
}

EntrypointId WinSettingsPageRootItem::uniqueId() const { return EntrypointId("windows-settings", m_page.id); }

AccessoryList WinSettingsPageRootItem::accessories() const {
  return {{.text = "Settings", .color = SemanticColor::TextMuted}};
}

std::vector<QString> WinSettingsPageRootItem::keywords() const {
  std::vector<QString> words;
  for (const QString &word : QString::fromUtf8(m_page.keywords).split(' ', Qt::SkipEmptyParts)) {
    words.emplace_back(word);
  }
  return words;
}

std::vector<std::pair<QString, QString>> WinSettingsPageRootItem::settingsMetadata() const {
  return {{"Name", m_page.title}, {"Category", m_page.category}, {"URL", m_page.url}};
}

std::unique_ptr<ActionPanelState>
WinSettingsPageRootItem::newActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto utils = panel->createSection();
  auto itemSection = panel->createSection();

  auto open = new OpenWindowsSettingAction(QString("Open %1 Settings").arg(m_page.title), iconUrl(),
                                           QString::fromUtf8(m_page.url));
  mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));

  utils->addAction(new CopyToClipboardAction(Clipboard::Text(m_page.url), "Copy URL"));

  for (const auto &action : RootSearchActionGenerator::generateActions(*this, metadata)) {
    itemSection->addAction(action);
  }

  panel->setTitle(m_page.title);
  return panel;
}

QString WinSettingsRootProvider::uniqueId() const { return "windows-settings"; }

QString WinSettingsRootProvider::displayName() const { return "Windows Settings"; }

QString WinSettingsRootProvider::description() const { return "Pages of the Windows Settings app."; }

ImageURL WinSettingsRootProvider::icon() const { return ImageURL::winShellIcon(SETTINGS_APP_PARSING_NAME); }

RootProvider::Type WinSettingsRootProvider::type() const { return RootProvider::Type::GroupProvider; }

std::vector<std::shared_ptr<RootItem>> WinSettingsRootProvider::loadItems() const {
  std::vector<std::shared_ptr<RootItem>> items;
  items.reserve(std::size(SETTINGS_PAGES));

  for (const auto &page : SETTINGS_PAGES) {
    items.emplace_back(std::make_shared<WinSettingsPageRootItem>(page));
  }

  return items;
}
