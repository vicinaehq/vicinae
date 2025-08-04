#include "extension/extension-command.hpp"
#include "command.hpp"
#include "extension/extension-command-runtime.hpp"
#include "../ui/image/url.hpp"
#include <filesystem>

QString ExtensionCommand::extensionId() const { return _extensionId; }
void ExtensionCommand::setExtensionId(const QString &text) { _extensionId = text; }

const QString &ExtensionCommand::extensionIcon() const { return _extensionIcon; }
void ExtensionCommand::setExtensionIcon(const QString &icon) { _extensionIcon = icon; }

CommandContext *ExtensionCommand::createContext(const std::shared_ptr<AbstractCmd> &command) const {
  return new ExtensionCommandRuntime(static_pointer_cast<ExtensionCommand>(command));
}

CommandType ExtensionCommand::type() const { return CommandType::CommandTypeExtension; }

CommandMode ExtensionCommand::mode() const { return m_command.mode; }

QString ExtensionCommand::uniqueId() const { return QString("%1.%2").arg(_extensionId).arg(m_command.name); }

QString ExtensionCommand::commandId() const { return m_command.name; }

QString ExtensionCommand::name() const { return m_command.title; }

QString ExtensionCommand::repositoryName() const { return _extensionTitle; }

void ExtensionCommand::setAssetPath(const std::filesystem::path &path) { _assetPath = path; }

void ExtensionCommand::setExtensionTitle(const QString &title) { _extensionTitle = title; }

ImageURL ExtensionCommand::iconUrl() const {
  if (auto icon = m_command.icon) {
    auto commandIconPath = _assetPath / icon->toStdString();

    if (std::filesystem::exists(commandIconPath)) { return LocalOmniIconUrl(commandIconPath); }
  }

  auto extensionIconUrl = _assetPath / _extensionIcon.toStdString();

  if (std::filesystem::exists(extensionIconUrl)) { return LocalOmniIconUrl(extensionIconUrl); }

  return BuiltinOmniIconUrl("hammer").setBackgroundTint(SemanticColor::Blue);
}
