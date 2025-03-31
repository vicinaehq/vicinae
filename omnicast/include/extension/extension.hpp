#pragma once
#include "command-database.hpp"
#include "omni-icon.hpp"
#include "preference.hpp"
#include <filesystem>
#include <qjsonobject.h>
#include <qstring.h>
#include <filesystem>
#include <qstringview.h>

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
  QString _sessionId;
  QString _title;
  QString _icon;
  std::filesystem::path _path;
  PreferenceList _preferences;
  std::vector<std::shared_ptr<AbstractCmd>> _commands;

  explicit Extension(const QJsonObject &object);

public:
  Extension() {}

  static Extension fromObject(const QJsonObject &object);

  Preference parsePreferenceFromObject(const QJsonObject &obj) {
    auto type = obj["type"].toString();
    BasePreference base;

    base.title = obj["title"].toString();
    base.description = obj["description"].toString();
    base.name = obj["name"].toString();
    base.placeholder = obj["placeholder"].toString();
    base.required = obj["required"].toBool();

    if (type == "textfield") {
      TextFieldPreference textField(base);

      textField.defaultValue = obj["default"].toString();

      return textField;
    }

    if (type == "password") {
      PasswordPreference password(base);

      password.defaultValue = obj["default"].toString();

      return password;
    }

    if (type == "checkbox") {
      CheckboxPreference preference(base);

      preference.label = obj["label"].toString();
      preference.defaultValue = obj["default"].toBool();

      return preference;
    }

    if (type == "dropdown") {
      DropdownPreference preference(base);

      // TODO: parse data
      preference.data = {};
      preference.defaultValue = obj["default"].toString();

      return preference;
    }

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

    return BasePreference(base);
  }

  QString id() const override;
  QString name() const override;
  OmniIconUrl iconUrl() const override;
  std::filesystem::path assetDirectory() const;
  std::filesystem::path installedPath() const;
  std::vector<Preference> preferences() const override;
  std::vector<std::shared_ptr<AbstractCmd>> commands() const override;
};
