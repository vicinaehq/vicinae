#include "extension/extension.hpp"
#include "extension/extension-command.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include "services/extension-registry/extension-registry.hpp"
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qlogging.h>

OmniIconUrl Extension::iconUrl() const {
  auto fallback = BuiltinOmniIconUrl("hammer").setBackgroundTint(ColorTint::Blue);

  if (!m_manifest.icon.isEmpty()) {
    return LocalOmniIconUrl(assetDirectory() / m_manifest.icon.toStdString()).withFallback(fallback);
  }

  return fallback;
}

QString Extension::id() const { return m_manifest.id; }

QString Extension::name() const { return m_manifest.name; }

std::filesystem::path Extension::assetDirectory() const { return m_manifest.path / "assets"; }

std::filesystem::path Extension::installedPath() const { return m_manifest.path; }

PreferenceList Extension::preferences() const { return m_manifest.preferences; }

std::vector<std::shared_ptr<AbstractCmd>> Extension::commands() const { return m_commands; }

QString Extension::description() const { return m_manifest.description; };

const ExtensionManifest &Extension::manifest() const { return m_manifest; }

Extension::Extension(const ExtensionManifest &manifest) : m_manifest(manifest) {
  for (const auto &cmd : m_manifest.commands) {
    auto command = std::make_shared<ExtensionCommand>(cmd);

    command->setExtensionId(m_manifest.id);
    command->setAssetPath(assetDirectory());
    command->setExtensionTitle(m_manifest.name);
    command->setExtensionIcon(m_manifest.icon);
    command->setExtensionPreferences(m_manifest.preferences);
    m_commands.emplace_back(command);
  }
}
