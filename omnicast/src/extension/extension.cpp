#include "extension/extension.hpp"
#include "command-database.hpp"
#include "extension/extension-command.hpp"
#include "omni-icon.hpp"
#include <qjsonobject.h>
#include <qjsonarray.h>

OmniIconUrl Extension::iconUrl() const {
  if (!_icon.isEmpty()) { return LocalOmniIconUrl(assetDirectory() / _icon.toStdString()); }

  return BuiltinOmniIconUrl("hammer").setBackgroundTint(ColorTint::Blue);
}

QString Extension::id() const { return _id; }

QString Extension::name() const { return _id; }

std::filesystem::path Extension::assetDirectory() const { return _path / "assets"; }

std::filesystem::path Extension::installedPath() const { return _path; }

PreferenceList Extension::preferences() const { return _preferences; }

std::vector<std::shared_ptr<AbstractCommand>> Extension::commands() const { return _commands; }

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
    /*
Extension::CommandBase base{.name = cmdObj["name"].toString(),
                            .title = cmdObj["title"].toString(),
                            .subtitle = cmdObj["subtitle"].toString(),
                            .description = cmdObj["description"].toString(),
                            .mode = cmdObj["mode"].toString()};

if (base.mode == "view") { _commands.push_back(std::make_shared<ExtensionViewCommand>(base)); }

_commands.push_back(finalCmd);
    */
  }
}
