#include "extension/extension-command.hpp"
#include "command.hpp"
#include "common.hpp"
#include "extension/extension-command-runtime.hpp"
#include "../ui/image/url.hpp"
#include <filesystem>
#include <format>

QString ExtensionCommand::extensionId() const { return _extensionId; }
void ExtensionCommand::setExtensionId(const QString &text) { _extensionId = text; }

const QString &ExtensionCommand::extensionIcon() const { return _extensionIcon; }
void ExtensionCommand::setExtensionIcon(const QString &icon) { _extensionIcon = icon; }

CommandContext *ExtensionCommand::createContext(const std::shared_ptr<AbstractCmd> &command) const {
  return new ExtensionCommandRuntime(static_pointer_cast<ExtensionCommand>(command));
}

CommandType ExtensionCommand::type() const { return CommandType::CommandTypeExtension; }

CommandMode ExtensionCommand::mode() const { return m_command.mode; }

EntrypointId ExtensionCommand::uniqueId() const {
  std::string provider = std::format("@{}/{}", m_author.toStdString(), _extensionId.toStdString());
  return EntrypointId{provider, m_command.name.toStdString()};
}

QString ExtensionCommand::commandId() const { return m_command.name; }

QString ExtensionCommand::name() const { return m_command.title; }

QString ExtensionCommand::repositoryDisplayName() const { return _extensionTitle; }

QString ExtensionCommand::repositoryName() const { return m_extensionName; }

bool ExtensionCommand::isDefaultDisabled() const { return m_command.defaultDisabled; }

void ExtensionCommand::setPath(const std::filesystem::path &path) { m_path = path; }

void ExtensionCommand::setExtensionTitle(const QString &title) { _extensionTitle = title; }

void ExtensionCommand::setExtensionName(const QString &name) { m_extensionName = name; }

ImageURL ExtensionCommand::iconUrl() const {
  if (auto icon = m_command.icon) {
    auto commandIconPath = assetPath() / icon->toStdString();

    if (std::filesystem::exists(commandIconPath)) { return ImageURL::local(commandIconPath); }
  }

  auto extensionIconUrl = assetPath() / _extensionIcon.toStdString();

  if (std::filesystem::exists(extensionIconUrl)) { return ImageURL::local(extensionIconUrl); }

  return ImageURL::builtin("hammer").setBackgroundTint(SemanticColor::Blue);
}
