#include "extension/extension-command.hpp"
#include "command.hpp"
#include "extension/extension-command-runtime.hpp"
#include "extension/extension.hpp"
#include "omni-icon.hpp"
#include <filesystem>

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
  } else if (mode == "menu-bar") {
    _mode = CommandModeMenuBar;
  } else {
    _mode = CommandModeInvalid;
  }

  QJsonArray preferenceList = obj["preferences"].toArray();

  _preferences.reserve(preferenceList.size());

  for (const auto &preference : preferenceList) {
    _preferences.emplace_back(Extension::parsePreferenceFromObject(preference.toObject()));
  }

  QJsonArray argList = obj.value("arguments").toArray();

  m_arguments.reserve(argList.size());

  for (const auto &argument : argList) {
    m_arguments.emplace_back(Extension::parseArgumentFromObject(argument.toObject()));
  }
}

QString ExtensionCommand::extensionId() const { return _extensionId; }
void ExtensionCommand::setExtensionId(const QString &text) { _extensionId = text; }

const QString &ExtensionCommand::extensionIcon() const { return _extensionIcon; }
void ExtensionCommand::setExtensionIcon(const QString &icon) { _extensionIcon = icon; }

CommandContext *ExtensionCommand::createContext(const std::shared_ptr<AbstractCmd> &command) const {
  return new ExtensionCommandRuntime(static_pointer_cast<ExtensionCommand>(command));
}

CommandType ExtensionCommand::type() const { return CommandType::CommandTypeExtension; }

CommandMode ExtensionCommand::mode() const { return _mode; }

QString ExtensionCommand::uniqueId() const { return QString("%1.%2").arg(_extensionId).arg(_id); }

QString ExtensionCommand::commandId() const { return _id; }

QString ExtensionCommand::name() const { return _title; }

QString ExtensionCommand::repositoryName() const { return _extensionTitle; }

void ExtensionCommand::setAssetPath(const std::filesystem::path &path) { _assetPath = path; }

void ExtensionCommand::setExtensionTitle(const QString &title) { _extensionTitle = title; }

OmniIconUrl ExtensionCommand::iconUrl() const {
  if (!_icon.isEmpty()) {
    auto commandIconPath = _assetPath / _icon.toStdString();

    if (std::filesystem::exists(commandIconPath)) { return LocalOmniIconUrl(commandIconPath); }
  }

  auto extensionIconUrl = _assetPath / _extensionIcon.toStdString();

  if (std::filesystem::exists(extensionIconUrl)) { return LocalOmniIconUrl(extensionIconUrl); }

  return BuiltinOmniIconUrl("hammer").setBackgroundTint(ColorTint::Blue);
}
