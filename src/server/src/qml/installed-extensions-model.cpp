#include "installed-extensions-model.hpp"
#include "actions/extension/extension-actions.hpp"
#include "builtin_icon.hpp"
#include "clipboard-actions.hpp"
#include "extend/accessory-model.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "view-utils.hpp"

QString InstalledExtensionsModel::displayTitle(const ExtensionManifest &m) const { return m.title; }

QString InstalledExtensionsModel::displaySubtitle(const ExtensionManifest &m) const { return m.description; }

QString InstalledExtensionsModel::displayIconSource(const ExtensionManifest &m) const {
  if (!m.icon.isEmpty()) {
    auto iconPath = m.path / "assets" / m.icon.toStdString();
    return imageSourceFor(ImageURL::local(iconPath).withFallback(ImageURL::builtin("plug")));
  }
  return imageSourceFor(ImageURL::builtin("plug"));
}

QVariantList InstalledExtensionsModel::displayAccessory(const ExtensionManifest &m) const {
  if (m.isFromRaycastStore()) {
    return qml::accessoriesToVariantList({{.data = AccessoryModel::Tag(SemanticColor::Red, "Raycast"),
                                           .icon = ExtensionImageModel{.source = QString("raycast")}}});
  }
  if (m.isFromVicinaeStore()) {
    return qml::accessoriesToVariantList({{.data = AccessoryModel::Tag(SemanticColor::Orange, "Vicinae"),
                                           .icon = ExtensionImageModel{.source = QString("vicinae")}}});
  }
  if (m.isLocal()) {
    return qml::accessoriesToVariantList({{.data = AccessoryModel::Tag(SemanticColor::Blue, "Local")}});
  }
  return {};
}

std::unique_ptr<ActionPanelState>
InstalledExtensionsModel::buildActionPanel(const ExtensionManifest &m) const {
  auto panel = std::make_unique<ListActionPanelState>();
  panel->setTitle(m.title);

  auto primary = panel->createSection();
  primary->addAction(new UninstallExtensionAction(m.id));

  auto utils = panel->createSection("Copy");

  auto copyName = new CopyToClipboardAction(Clipboard::Text(m.name), "Copy Name");
  copyName->setShortcut(Keybind::CopyNameAction);
  utils->addAction(copyName);

  auto copyId = new CopyToClipboardAction(Clipboard::Text(m.id), "Copy ID");
  utils->addAction(copyId);

  auto copyPath = new CopyToClipboardAction(Clipboard::Text(m.path.c_str()), "Copy Path");
  copyPath->setShortcut(Keybind::CopyPathAction);
  utils->addAction(copyPath);

  auto copyAuthor = new CopyToClipboardAction(Clipboard::Text(m.author), "Copy Author");
  utils->addAction(copyAuthor);

  return panel;
}

QString InstalledExtensionsModel::sectionLabel() const {
  return QStringLiteral("Installed Extensions ({count})");
}
