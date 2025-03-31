#include "extension/extension-command.hpp"
#include "command.hpp"
#include "extension/extension-command-context.hpp"

ExtensionCommand ExtensionCommand::fromJson(const QJsonObject &obj) { return obj; }

ExtensionCommand::ExtensionCommand(const QJsonObject &obj) {
  _id = obj["name"].toString();
  _title = obj["title"].toString();
  _icon = obj["icon"].toString();

  auto mode = obj["mode"].toString();

  if (mode == "view") {
    _mode = CommandModeView;
  } else if (mode == "no-view") {
    _mode = CommandModeNoView;
  }
}

const QString &ExtensionCommand::extensionSessionId() const { return _sessionId; }
void ExtensionCommand::setExtensionSessionId(const QString &text) { _sessionId = text; }

const QString &ExtensionCommand::extensionIcon() const { return _extensionIcon; }
void ExtensionCommand::setExtensionIcon(const QString &icon) { _extensionIcon = icon; }

CommandContext *ExtensionCommand::createContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command,
                                                const QString &query) const {
  return new ExtensionCommandContext(app, command);
}

CommandType ExtensionCommand::type() const { return CommandType::CommandTypeExtension; }

CommandMode ExtensionCommand::mode() const { return _mode; }

QString ExtensionCommand::id() const { return _id; }

QString ExtensionCommand::name() const { return _title; }

QString ExtensionCommand::repositoryName() const { return _extensionTitle; }

void ExtensionCommand::setAssetPath(const std::filesystem::path &path) { _assetPath = path; }

void ExtensionCommand::setExtensionTitle(const QString &title) { _extensionTitle = title; }

OmniIconUrl ExtensionCommand::iconUrl() const {
  if (!_icon.isEmpty()) { return LocalOmniIconUrl(_assetPath / _icon.toStdString()); }
  if (!_extensionIcon.isEmpty()) { return LocalOmniIconUrl(_assetPath / _extensionIcon.toStdString()); }

  return BuiltinOmniIconUrl("hammer", ColorTint::Blue);
}
