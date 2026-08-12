#include "installed-extensions-model.hpp"
#include "actions/extension/extension-actions.hpp"
#include "clipboard-actions.hpp"
#include "keyboard/keybind.hpp"
#include "navigation-controller.hpp"
#include "theme/colors.hpp"
#include "ui/image/url.hpp"
#include "view-utils.hpp"

QString InstalledExtensionsSection::displayTitle(const ExtensionManifest &m) const { return m.title; }

QString InstalledExtensionsSection::displaySubtitle(const ExtensionManifest &m) const {
  return m.description;
}

std::optional<ImageURL> InstalledExtensionsSection::displayIcon(const ExtensionManifest &m) const {
  if (!m.icon.isEmpty()) {
    auto iconPath = m.path / "assets" / m.icon.toStdString();
    return ImageURL::local(iconPath).withFallback(ImageURL::builtin(BuiltinIcon::Plug));
  }
  return ImageURL::builtin(BuiltinIcon::Plug);
}

AccessoryList InstalledExtensionsSection::displayAccessories(const ExtensionManifest &m) const {
  if (m.isFromRaycastStore()) {
    return {{.text = "Raycast",
             .color = SemanticColor::Red,
             .fillBackground = true,
             .icon = ImageURL::builtin(BuiltinIcon::Raycast)}};
  }
  if (m.isFromVicinaeStore()) {
    return {{.text = "Vicinae",
             .color = SemanticColor::Accent,
             .fillBackground = true,
             .icon = ImageURL::builtin(BuiltinIcon::Vicinae)}};
  }
  if (m.isLocal()) {
    return {{.text = tr("Local"),
             .color = SemanticColor::Cyan,
             .fillBackground = true,
             .icon = ImageURL::builtin(BuiltinIcon::Box)}};
  }
  return {};
}

std::unique_ptr<ActionPanelState>
InstalledExtensionsSection::buildActionPanel(const ExtensionManifest &m) const {
  auto panel = std::make_unique<ListActionPanelState>();
  panel->setTitle(m.title);

  auto primary = panel->createSection();
  primary->addAction(new UninstallExtensionAction(m.id));

  auto utils = panel->createSection(tr("Copy"));

  auto copyName = new CopyToClipboardAction(Clipboard::Text(m.name), tr("Copy Name"));
  copyName->setShortcut(Keybind::CopyNameAction);
  utils->addAction(copyName);

  auto copyId = new CopyToClipboardAction(Clipboard::Text(m.id), tr("Copy ID"));
  utils->addAction(copyId);

  auto copyPath =
      new CopyToClipboardAction(Clipboard::Text(QString::fromStdString(m.path.string())), tr("Copy Path"));
  copyPath->setShortcut(Keybind::CopyPathAction);
  utils->addAction(copyPath);

  auto copyAuthor = new CopyToClipboardAction(Clipboard::Text(m.author), tr("Copy Author"));
  utils->addAction(copyAuthor);

  return panel;
}
