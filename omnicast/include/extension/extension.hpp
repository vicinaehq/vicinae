#pragma once
#include "omni-icon.hpp"
#include <qjsonobject.h>
#include <qstring.h>
#include <filesystem>
#include <qstringview.h>

class Extension {
public:
  enum ArgumentType { ArgumentText, ArgumentPassword, ArgumentDropdown };

  struct Argument {
    QString name;
    ArgumentType type;
    QString placeholder;
    bool required;
  };

  struct Command {
    QString name;
    QString title;
    QString subtitle;
    QString description;
    QString mode;
    QString extensionId;
  };

  enum PreferenceType {
    TextFieldPreferenceType,
    PasswordPreferenceType,
    CheckboxPreferenceType,
    DropdownPreferenceType,
    AppPickerPreferenceType,
    FilePreferenceType,
    DirectoryPreferenceType,
  };

  struct BasePreference {
    QString name;
    QString title;
    QString description;
    PreferenceType type;
    bool required;
    QString placeholder;
  };

  struct TextFieldPreference : BasePreference {
    QString defaultValue;

    explicit TextFieldPreference(const BasePreference &base) : BasePreference(base) {}
  };

  struct PasswordPreference : BasePreference {
    QString defaultValue;
  };

  struct CheckboxPreference : BasePreference {
    QString label;
    bool defaultValue;
  };

  struct AppPickerPreference : BasePreference {
    QString defaultValue;
  };

  struct FilePreference : BasePreference {
    std::filesystem::path defaultValue;
  };

  struct DirectoryPreference : BasePreference {
    std::filesystem::path defaultValue;
  };

  struct DropdownPreference : BasePreference {
    struct DropdownOption {
      QString title;
      QString value;
    };

    std::vector<DropdownOption> data;
    QString defaultValue;
  };

  using Preference =
      std::variant<BasePreference, TextFieldPreference, CheckboxPreference, DropdownPreference,
                   PasswordPreference, AppPickerPreference, FilePreference, DirectoryPreference>;
  using PreferenceList = std::vector<Preference>;
  using ArgumentList = std::vector<Argument>;

private:
  QString _id;
  QString _icon;
  std::filesystem::path _path;
  PreferenceList _preferences;
  std::vector<Command> _commands;

  explicit Extension(const QJsonObject &object);

public:
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

  QStringView id() const;
  OmniIconUrl iconUrl() const;
  std::filesystem::path assetDirectory() const;
  std::filesystem::path installedPath() const;
  const PreferenceList &preferences() const;
  const std::vector<Command> &commands() const;
};
