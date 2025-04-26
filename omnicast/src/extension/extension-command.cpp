#include "extension/extension-command.hpp"
#include "command-database.hpp"
#include "command.hpp"
#include "extension/extension-command-context.hpp"
#include "extension/extension-command-runtime.hpp"
#include "omni-icon.hpp"

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
    _preferences.push_back(Extension::parsePreferenceFromObject(preference.toObject()));
  }
}

const QString &ExtensionCommand::extensionId() const { return _extensionId; }
void ExtensionCommand::setExtensionId(const QString &text) { _extensionId = text; }

const QString &ExtensionCommand::extensionIcon() const { return _extensionIcon; }
void ExtensionCommand::setExtensionIcon(const QString &icon) { _extensionIcon = icon; }

CommandContext *ExtensionCommand::createContext(AppWindow &app, const std::shared_ptr<AbstractCmd> &command,
                                                const QString &query) const {
  return new ExtensionCommandRuntime(app, static_pointer_cast<ExtensionCommand>(command));
}

CommandType ExtensionCommand::type() const { return CommandType::CommandTypeExtension; }

CommandMode ExtensionCommand::mode() const { return _mode; }

QString ExtensionCommand::id() const { return _id; }

QString ExtensionCommand::name() const { return _title; }

QString ExtensionCommand::repositoryName() const { return _extensionTitle; }

void ExtensionCommand::setAssetPath(const std::filesystem::path &path) { _assetPath = path; }

void ExtensionCommand::setExtensionTitle(const QString &title) { _extensionTitle = title; }

OmniIconUrl ExtensionCommand::iconUrl() const {
  auto fallback = BuiltinOmniIconUrl("hammer").setBackgroundTint(ColorTint::Blue);

  if (!_icon.isEmpty()) { return LocalOmniIconUrl(_assetPath / _icon.toStdString()).withFallback(fallback); }
  if (!_extensionIcon.isEmpty()) {
    return LocalOmniIconUrl(_assetPath / _extensionIcon.toStdString()).withFallback(fallback);
  }

  return fallback;
}
