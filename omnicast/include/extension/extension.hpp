#pragma once
#include "command-database.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include <filesystem>
#include <qjsonobject.h>
#include <qstring.h>
#include <filesystem>
#include <qstringview.h>
#include <qjsonarray.h>

class Extension : public AbstractCommandRepository {
public:
  struct CommandBase : public BuiltinCommand {
    QString _name;
    QString title;
    QString subtitle;
    QString description;
    QString mode;
  };

private:
  QString _id;
  QString _name;
  QString _title;
  QString _icon;
  std::filesystem::path _path;
  PreferenceList _preferences;
  std::vector<std::shared_ptr<AbstractCmd>> _commands;

  explicit Extension(const QJsonObject &object);

public:
  Extension() {}

  static Extension fromObject(const QJsonObject &object);

  static std::shared_ptr<BasePreference> parsePreferenceFromObject(const QJsonObject &obj) {
    auto type = obj["type"].toString();
    BasePreference base;

    base.setTitle(obj["title"].toString());
    base.setDescription(obj["description"].toString());
    base.setName(obj["name"].toString());
    base.setPlaceholder(obj["placeholder"].toString());
    base.setRequired(obj["required"].toBool());

    if (type == "textfield") {
      auto textField = std::make_shared<TextFieldPreference>(base);

      if (obj.contains("default")) { textField->setDefaultValue(obj.value("default").toString()); }

      return textField;
    }

    if (type == "password") {
      auto password = std::make_shared<PasswordPreference>(base);

      if (obj.contains("default")) { password->setDefaultValue(obj.value("default").toString()); }

      return password;
    }

    if (type == "checkbox") {
      auto checkbox = std::make_shared<CheckboxPreference>(base);

      if (obj.contains("default")) { checkbox->setDefaultValue(obj.value("default").toBool()); }

      checkbox->setLabel(obj["label"].toString());

      return checkbox;
    }

    if (type == "dropdown") {
      auto dropdown = std::make_shared<DropdownPreference>(base);
      auto data = obj["data"].toArray();
      std::vector<DropdownPreference::Option> options;

      options.reserve(data.size());

      for (const auto &child : data) {
        auto obj = child.toObject();

        options.push_back({.title = obj["title"].toString(), .value = obj["value"].toString()});
      }

      dropdown->setOptions(options);

      if (obj.contains("default")) { dropdown->setDefaultValue(obj.value("default").toString()); }

      return dropdown;
    }

    /*
if (type == "appPicker") {
  AppPickerPreference preference(base);

  preference.defaultValue = obj["default"].toString();

  return preference;
}


if (type == "file") {
  FilePreference preference(base);

  preference.defaultValue = obj["default"].toString().toStdString();

  return preference;
}

if (type == "directory") {
  DirectoryPreference preference(base);

  preference.defaultValue = obj["default"].toString().toStdString();

  return preference;
}
    */

    return nullptr;
  }

  QString id() const override;
  QString name() const override;
  OmniIconUrl iconUrl() const override;
  std::filesystem::path assetDirectory() const;
  std::filesystem::path installedPath() const;
  PreferenceList preferences() const override;
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override;
};
