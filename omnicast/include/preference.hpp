#pragma once
#include <qjsonvalue.h>
#include <qstring.h>
#include <filesystem>

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
  QString _name;
  QString _title;
  QString _description;
  PreferenceType _type;
  bool _required;
  QString _placeholder;

  const QString &name() const { return _name; }
  const QString &title() const { return _title; }
  const QString &description() const { return _description; }
  const QString &placeholder() const { return _placeholder; }
  PreferenceType type() const { return _type; }
  bool isDropdownType() const { return _type == DropdownPreferenceType; }
  bool isTextType() const { return _type == TextFieldPreferenceType || _type == PasswordPreferenceType; }
  bool isPasswordType() const { return _type == PasswordPreferenceType; }
  bool isRequired() const { return _required; }

  auto &setName(const QString &name) {
    _name = name;
    return *this;
  }
  auto &setTitle(const QString &title) {
    _title = title;
    return *this;
  }
  auto &setDescription(const QString &description) {
    _description = description;
    return *this;
  }
  auto &setRequired(bool required) {
    _required = true;
    return *this;
  }
  auto &setType(PreferenceType type) {
    _type = type;
    return *this;
  }
  auto &setPlaceholder(const QString &placeholder) {
    _placeholder = placeholder;
    return *this;
  }

  // used to populate the preference values object when the default has to be used
  virtual QJsonValue defaultValueAsJson() const { return {}; }
};

struct TextFieldPreference : BasePreference {
  std::optional<QString> _defaultValue;

  TextFieldPreference(const BasePreference &preference = {}) : BasePreference(preference) {
    setType(PreferenceType::TextFieldPreferenceType);
  }

  void setDefaultValue(const std::optional<QString> &value) { _defaultValue = value; }

  QJsonValue defaultValueAsJson() const override { return _defaultValue ? (*_defaultValue) : QJsonValue(); }
};

struct PasswordPreference : public TextFieldPreference {
  PasswordPreference(const BasePreference &base) : TextFieldPreference(base) {
    setType(PreferenceType::PasswordPreferenceType);
  }
};

struct CheckboxPreference : BasePreference {
  QString _label;
  bool defaultValue;

  void setLabel(const QString &label) { _label = label; }
  void setDefaultValue(bool value) { defaultValue = value; }

  QJsonValue defaultValueAsJson() const override { return QJsonValue(defaultValue); }

  CheckboxPreference(const BasePreference &base = {}) : BasePreference(base), defaultValue(false) {
    setType(CheckboxPreferenceType);
  }
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

class DropdownPreference : public BasePreference {
public:
  struct Option {
    QString title;
    QString value;
  };

private:
  std::vector<Option> _options;
  std::optional<QString> _defaultValue;

public:
  void setOptions(const std::vector<Option> &options) { _options = options; }
  const std::vector<Option> &options() const { return _options; }
  void setDefaultValue(const QString &value) { _defaultValue = value; }

  QJsonValue defaultValueAsJson() const override { return _defaultValue ? *_defaultValue : QJsonValue(); }

  DropdownPreference(const BasePreference base = {}) : BasePreference(base) {
    setType(DropdownPreferenceType);
  }
};

using PreferenceList = std::vector<std::shared_ptr<BasePreference>>;
