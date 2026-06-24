#include "root-search/macos-settings/macos-settings-root-provider.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "clipboard-actions.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action-panel-state.hpp"
#include "ui/image/url.hpp"
#include "vicinae.hpp"

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

#include <system_error>

namespace fs = std::filesystem;

namespace {

constexpr const char *EXTENSIONS_DIR = "/System/Library/ExtensionKit/Extensions";
constexpr const char *SETTINGS_EXTENSION_POINT = "com.apple.Settings.extension.ui";

NSString *toNSString(const QString &s) {
  return [NSString stringWithCharacters:reinterpret_cast<const unichar *>(s.utf16()) length:s.size()];
}

NSString *toNSString(const fs::path &p) { return [NSString stringWithUTF8String:p.c_str()]; }

QString fromNSString(NSString *s) { return s ? QString::fromNSString(s) : QString(); }

class OpenSettingsPaneAction : public AbstractAction {
public:
  OpenSettingsPaneAction(const QString &title, const ImageURL &icon, QString url)
      : AbstractAction(title, icon), m_url(std::move(url)) {}

  void execute(ApplicationContext *ctx) override {
    bool opened = false;
    @autoreleasepool {
      NSURL *url = [NSURL URLWithString:toNSString(m_url)];
      if (url) opened = [[NSWorkspace sharedWorkspace] openURL:url];
    }

    if (!opened) {
      ctx->services->toastService()->failure("Failed to open System Settings");
      return;
    }

    ctx->navigation->closeWindow();
    ctx->navigation->clearSearchText();
  }

private:
  QString m_url;
};

std::optional<MacSettingsPane> readPane(const fs::path &path, NSString *settingsExtensionPoint) {
  @autoreleasepool {
    NSBundle *bundle = [NSBundle bundleWithPath:toNSString(path)];
    if (!bundle) return std::nullopt;

    NSDictionary *info = bundle.infoDictionary;
    NSDictionary *attrs = info[@"EXAppExtensionAttributes"];
    NSString *pointId = attrs[@"EXExtensionPointIdentifier"];
    if (![pointId isEqualToString:settingsExtensionPoint]) return std::nullopt;

    NSDictionary *localized = bundle.localizedInfoDictionary;
    auto pick = [&](NSString *key) -> NSString * {
      NSString *value = localized[key];
      if (value.length > 0) return value;
      value = info[key];
      return value.length > 0 ? value : nil;
    };

    NSString *displayName = pick(@"CFBundleDisplayName");
    if (!displayName) displayName = pick(@"CFBundleName");
    if (!displayName) return std::nullopt;

    MacSettingsPane pane;
    pane.bundlePath = path;
    pane.displayName = fromNSString(displayName);
    pane.bundleId = fromNSString(info[@"CFBundleIdentifier"]);

    NSDictionary *settingsAttrs = attrs[@"SettingsExtensionAttributes"];
    id legacy = settingsAttrs[@"legacyBundleIdentifier"];
    if ([legacy isKindOfClass:[NSString class]]) {
      pane.legacyBundleId = fromNSString(legacy);
    } else if ([legacy isKindOfClass:[NSArray class]]) {
      for (id entry in (NSArray *)legacy) {
        if ([entry isKindOfClass:[NSString class]] && [(NSString *)entry length] > 0) {
          pane.legacyBundleId = fromNSString(entry);
          break;
        }
      }
    }

    // Without any identifier, we cannot build an x-apple.systempreferences: URL
    // nor a stable EntrypointId, so the item is useless.
    if (pane.bundleId.isEmpty() && pane.legacyBundleId.isEmpty()) return std::nullopt;

    return pane;
  }
}

std::vector<MacSettingsPane> discoverPanes() {
  std::vector<MacSettingsPane> panes;
  std::error_code ec;

  fs::directory_iterator it(EXTENSIONS_DIR, fs::directory_options::skip_permission_denied, ec);
  if (ec) return panes;

  NSString *settingsExtensionPoint = [NSString stringWithUTF8String:SETTINGS_EXTENSION_POINT];

  for (const auto &entry : it) {
    const auto &p = entry.path();
    if (!p.filename().string().ends_with(".appex")) continue;
    if (auto pane = readPane(p, settingsExtensionPoint)) panes.emplace_back(std::move(*pane));
  }

  return panes;
}

QString paneUrl(const MacSettingsPane &pane) {
  const QString &id = !pane.legacyBundleId.isEmpty() ? pane.legacyBundleId : pane.bundleId;
  return QStringLiteral("x-apple.systempreferences:") + id;
}

} // namespace

QString MacSettingsRootItem::title() const { return m_pane.displayName; }

QString MacSettingsRootItem::typeDisplayName() const { return "System Settings"; }

ImageURL MacSettingsRootItem::iconUrl() const { return ImageURL::macBundle(m_pane.bundlePath); }

EntrypointId MacSettingsRootItem::uniqueId() const {
  return EntrypointId("macos-settings", m_pane.bundleId.toStdString());
}

AccessoryList MacSettingsRootItem::accessories() const {
  return {{.text = "System Settings", .color = SemanticColor::TextMuted}};
}

std::vector<std::pair<QString, QString>> MacSettingsRootItem::settingsMetadata() const {
  std::vector<std::pair<QString, QString>> meta;
  meta.reserve(4);
  meta.emplace_back("Name", m_pane.displayName);
  meta.emplace_back("Bundle ID", m_pane.bundleId);
  if (!m_pane.legacyBundleId.isEmpty()) meta.emplace_back("Legacy ID", m_pane.legacyBundleId);
  meta.emplace_back("Where", QString::fromStdString(m_pane.bundlePath.string()));
  return meta;
}

std::unique_ptr<ActionPanelState>
MacSettingsRootItem::newActionPanel(ApplicationContext *ctx, const RootItemMetadata &metadata) const {
  auto panel = std::make_unique<ListActionPanelState>();
  auto mainSection = panel->createSection();
  auto utils = panel->createSection();
  auto itemSection = panel->createSection();

  const QString url = paneUrl(m_pane);
  auto open =
      new OpenSettingsPaneAction(QString("Open %1 Settings").arg(m_pane.displayName), iconUrl(), url);
  mainSection->addAction(new DefaultActionWrapper(uniqueId(), open));

  utils->addAction(new CopyToClipboardAction(Clipboard::Text(url), "Copy URL"));
  utils->addAction(new CopyToClipboardAction(Clipboard::Text(m_pane.bundleId), "Copy Bundle ID"));

  for (const auto &action : RootSearchActionGenerator::generateActions(*this, metadata)) {
    itemSection->addAction(action);
  }

  panel->setTitle(m_pane.displayName);
  return panel;
}

QString MacSettingsRootProvider::uniqueId() const { return "macos-settings"; }

QString MacSettingsRootProvider::displayName() const { return "System Settings"; }

ImageURL MacSettingsRootProvider::icon() const {
  return ImageURL::builtin("cog").setBackgroundTint(Omnicast::ACCENT_COLOR);
}

RootProvider::Type MacSettingsRootProvider::type() const { return RootProvider::Type::GroupProvider; }

std::vector<std::shared_ptr<RootItem>> MacSettingsRootProvider::loadItems() const {
  auto panes = discoverPanes();
  std::vector<std::shared_ptr<RootItem>> items;
  items.reserve(panes.size());
  for (auto &pane : panes) {
    items.emplace_back(std::make_shared<MacSettingsRootItem>(std::move(pane)));
  }
  return items;
}
