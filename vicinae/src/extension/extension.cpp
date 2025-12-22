#include "extension/extension.hpp"
#include "extension/extension-command.hpp"
#include "ui/image/url.hpp"
#include "preference.hpp"
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qlogging.h>

ImageURL Extension::iconUrl() const {
  auto fallback = ImageURL::builtin("hammer").setBackgroundTint(SemanticColor::Blue);

  if (!m_manifest.icon.isEmpty()) {
    return ImageURL::local(assetDirectory() / m_manifest.icon.toStdString()).withFallback(fallback);
  }

  return fallback;
}

QString Extension::id() const { return QString("@%1/%2").arg(m_manifest.author).arg(m_manifest.id); }

QString Extension::displayName() const { return m_manifest.title; }

QString Extension::name() const { return m_manifest.name; }

std::filesystem::path Extension::assetDirectory() const { return m_manifest.path / "assets"; }

std::filesystem::path Extension::installedPath() const { return m_manifest.path; }

PreferenceList Extension::preferences() const { return m_manifest.preferences; }

QString Extension::author() const { return m_manifest.author; }

std::vector<std::shared_ptr<AbstractCmd>> Extension::commands() const { return m_commands; }

QString Extension::description() const { return m_manifest.description; };

const ExtensionManifest &Extension::manifest() const { return m_manifest; }

Extension::Extension(const ExtensionManifest &manifest) : m_manifest(manifest) {
  for (const auto &cmd : m_manifest.commands) {
    auto command = std::make_shared<ExtensionCommand>(cmd);

    command->setExtensionId(m_manifest.id);
    command->setPath(m_manifest.path);
    command->setExtensionTitle(m_manifest.title);
    command->setExtensionIcon(m_manifest.icon);
    command->setExtensionPreferences(m_manifest.preferences);
    command->setExtensionName(m_manifest.name);
    command->setAuthor(author());
    m_commands.emplace_back(command);
  }
}
