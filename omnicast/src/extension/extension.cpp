#include "extension/extension.hpp"
#include "extension/extension-command.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qlogging.h>

OmniIconUrl Extension::iconUrl() const {
  auto fallback = BuiltinOmniIconUrl("hammer").setBackgroundTint(ColorTint::Blue);

  if (!m_icon.isEmpty()) {
    return LocalOmniIconUrl(assetDirectory() / m_icon.toStdString()).withFallback(fallback);
  }

  return fallback;
}

QString Extension::id() const { return m_id; }

QString Extension::name() const { return m_id; }

std::filesystem::path Extension::assetDirectory() const { return m_path / "assets"; }

std::filesystem::path Extension::installedPath() const { return m_path; }

PreferenceList Extension::preferences() const { return m_preferences; }

std::vector<std::shared_ptr<AbstractCmd>> Extension::commands() const { return m_commands; }

Extension Extension::fromObject(const QJsonObject &obj) { return Extension(obj); }

QString Extension::description() const { return m_description; };

CommandArgument Extension::parseArgumentFromObject(const QJsonObject &obj) {
  CommandArgument arg;
  QString type = obj.value("type").toString();

  if (type == "text") arg.type = CommandArgument::Text;
  if (type == "password") arg.type = CommandArgument::Password;
  if (type == "dropdown") arg.type = CommandArgument::Dropdown;

  arg.name = obj.value("name").toString();
  arg.placeholder = obj.value("placeholder").toString();
  arg.required = obj.value("required").toBool(false);

  if (type == "dropdown") {
    auto data = obj.value("data").toArray();
    std::vector<CommandArgument::DropdownData> options;

    options.reserve(data.size());

    for (const auto &child : data) {
      auto obj = child.toObject();

      options.push_back({.title = obj["title"].toString(), .value = obj["value"].toString()});
    }

    arg.data = options;
  }

  return arg;
}

Preference Extension::parsePreferenceFromObject(const QJsonObject &obj) {
  auto type = obj["type"].toString();
  Preference base;

  base.setTitle(obj["title"].toString());
  base.setDescription(obj["description"].toString());
  base.setName(obj["name"].toString());
  base.setPlaceholder(obj["placeholder"].toString());
  base.setRequired(obj["required"].toBool());
  base.setDefaultValue(obj.value("default").toString());

  if (type == "textfield") { base.setData(Preference::TextData()); }
  if (type == "password") { base.setData(Preference::PasswordData()); }

  if (type == "checkbox") {
    auto checkbox = Preference::CheckboxData(obj["label"].toString());

    base.setData(checkbox);
  }

  if (type == "appPicker") {
    // XXX: implement later - if we really need it
  }

  if (type == "dropdown") {
    auto data = obj["data"].toArray();
    std::vector<Preference::DropdownData::Option> options;

    options.reserve(data.size());

    for (const auto &child : data) {
      auto obj = child.toObject();

      options.push_back({.title = obj["title"].toString(), .value = obj["value"].toString()});
    }

    base.setData(Preference::DropdownData{options});
  }

  return base;
}

Extension::Extension(const QJsonObject &obj) {
  m_id = obj["id"].toString();
  m_name = obj["name"].toString();
  m_title = obj["title"].toString();
  m_path = obj["path"].toString().toStdString();
  m_icon = obj["icon"].toString();
  m_description = obj.value("description").toString();

  QJsonArray commandList = obj["commands"].toArray();
  QJsonArray preferenceList = obj["preferences"].toArray();

  m_commands.reserve(commandList.size());
  m_preferences.reserve(preferenceList.size());

  qDebug() << "preferences" << preferenceList.size();

  for (const auto &preference : preferenceList) {
    auto pref = parsePreferenceFromObject(preference.toObject());

    if (!pref.isValid()) continue;

    m_preferences.push_back(pref);
  }

  for (const auto &cmd : commandList) {
    auto command = std::make_shared<ExtensionCommand>(ExtensionCommand::fromJson(cmd.toObject()));

    if (command->mode() == CommandModeInvalid || command->mode() == CommandModeMenuBar) {
      qDebug() << "Ignoring unsupported command mode";
      continue;
    }

    command->setExtensionId(m_id);
    command->setAssetPath(assetDirectory());
    command->setExtensionTitle(m_title);
    command->setExtensionIcon(m_icon);
    command->setExtensionPreferences(m_preferences);
    m_commands.push_back(command);
  }
}
