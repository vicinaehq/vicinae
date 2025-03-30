#include "extension/extension.hpp"
#include "omni-icon.hpp"
#include <qjsonobject.h>
#include <qjsonarray.h>

using PreferenceList = Extension::PreferenceList;
using Command = Extension::Command;

OmniIconUrl Extension::iconUrl() const {
  if (!_icon.isEmpty()) { return LocalOmniIconUrl(assetDirectory() / _icon.toStdString()); }

  return BuiltinOmniIconUrl("hammer").setBackgroundTint(ColorTint::Blue);
}

QStringView Extension::id() const { return _id; }

std::filesystem::path Extension::assetDirectory() const { return _path / "assets"; }

std::filesystem::path Extension::installedPath() const { return _path; }

const PreferenceList &Extension::preferences() const { return _preferences; }

const std::vector<Command> &Extension::commands() const { return _commands; }

Extension Extension::fromObject(const QJsonObject &obj) { return Extension(obj); }

Extension::Extension(const QJsonObject &obj) {
  _id = obj["sessionId"].toString();
  _path = obj["path"].toString().toStdString();
  _icon = obj["icon"].toString();

  QJsonArray commandList = obj["commands"].toArray();
  QJsonArray preferenceList = obj["preferences"].toArray();

  _commands.reserve(commandList.size());
  _preferences.reserve(preferenceList.size());

  qDebug() << "preferences" << preferenceList.size();

  for (const auto &preference : preferenceList) {
    _preferences.push_back(parsePreferenceFromObject(preference.toObject()));
  }

  for (const auto &cmd : commandList) {
    auto cmdObj = cmd.toObject();
    Extension::Command finalCmd{.name = cmdObj["name"].toString(),
                                .title = cmdObj["title"].toString(),
                                .subtitle = cmdObj["subtitle"].toString(),
                                .description = cmdObj["description"].toString(),
                                .mode = cmdObj["mode"].toString(),
                                .extensionId = _id};

    _commands.push_back(finalCmd);
  }
}
