#include "list-installed-extensions-view.hpp"
#include "actions/extension/extension-actions.hpp"
#include "clipboard-actions.hpp"
#include "keyboard/keybind.hpp"
#include "service-registry.hpp"
#include "services/clipboard/clipboard-service.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include "ui/image/url.hpp"
#include "ui/list-accessory/list-accessory.hpp"
#include "ui/omni-list/omni-list.hpp"
#include "ui/views/list-view.hpp"
#include <memory>

class InstalledExtensionItem : public SearchableListView::Actionnable {
  ExtensionManifest m_manifest;

public:
  InstalledExtensionItem(const ExtensionManifest &manifest) : m_manifest(manifest) {}

  bool hasUniformHeight() const override { return true; }
  QString generateId() const override { return m_manifest.id; }
  bool recyclable() const override { return false; }

  std::vector<QString> searchStrings() const override {
    return {m_manifest.title, m_manifest.description, m_manifest.author};
  }

  ItemData data() const override {
    ItemData data;
    data.name = m_manifest.title;
    data.subtitle = m_manifest.description;

    if (!m_manifest.icon.isEmpty()) {
      auto iconPath = m_manifest.path / "assets" / m_manifest.icon.toStdString();
      data.iconUrl = ImageURL::local(iconPath).withFallback(ImageURL::builtin("plug"));
    } else {
      data.iconUrl = ImageURL::builtin("plug");
    }

    ListAccessory tag;
    if (m_manifest.isFromRaycastStore()) {
      tag.text = "Raycast";
      tag.icon = ImageURL::builtin("raycast");
      tag.fillBackground = true;
      tag.color = SemanticColor::Red;
    } else if (m_manifest.isFromVicinaeStore()) {
      tag.text = "Vicinae";
      tag.icon = ImageURL::builtin("vicinae");
      tag.fillBackground = true;
      tag.color = SemanticColor::Accent;
    } else if (m_manifest.isLocal()) {
      tag.text = "Local";
      tag.icon = ImageURL::builtin("folder");
      tag.fillBackground = true;
      tag.color = SemanticColor::Orange;
    }

    data.accessories = {tag};

    return data;
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    panel->setTitle(m_manifest.title);

    // Primary section with uninstall
    auto primary = panel->createSection();
    auto uninstall = new UninstallExtensionAction(m_manifest.id);
    primary->addAction(uninstall);

    // Copy actions section
    auto utils = panel->createSection("Copy");

    auto copyName = new CopyToClipboardAction(Clipboard::Text(m_manifest.name), "Copy Name");
    copyName->setShortcut(Keybind::CopyNameAction);
    utils->addAction(copyName);

    auto copyId = new CopyToClipboardAction(Clipboard::Text(m_manifest.id), "Copy ID");
    utils->addAction(copyId);

    auto copyPath = new CopyToClipboardAction(Clipboard::Text(m_manifest.path.c_str()), "Copy Path");
    copyPath->setShortcut(Keybind::CopyPathAction);
    utils->addAction(copyPath);

    auto copyAuthor = new CopyToClipboardAction(Clipboard::Text(m_manifest.author), "Copy Author");
    utils->addAction(copyAuthor);

    return panel;
  }
};

ListInstalledExtensionsView::ListInstalledExtensionsView() {
  setSearchPlaceholderText("Search installed extensions...");

  // Listen for extension changes to refresh the list
  auto registry = ServiceRegistry::instance()->extensionRegistry();
  connect(registry, &ExtensionRegistry::extensionsChanged, this, [this]() {
    setData(initData());
    textChanged(searchText());
  });
}

QString ListInstalledExtensionsView::initialSearchPlaceholderText() const { return "Search extensions..."; }

SearchableListView::Data ListInstalledExtensionsView::initData() const {
  auto registry = ServiceRegistry::instance()->extensionRegistry();
  auto manifests = registry->scanAll();

  Data items;
  items.reserve(manifests.size());

  for (const auto &manifest : manifests) {
    items.push_back(std::make_shared<InstalledExtensionItem>(manifest));
  }

  return items;
}
