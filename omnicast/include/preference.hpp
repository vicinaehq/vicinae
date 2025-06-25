#pragma once
#include <qboxlayout.h>
#include <qjsonvalue.h>
#include <qnamespace.h>
#include <qstring.h>
#include <qwidget.h>
#include <variant>

class Preference {
public:
  struct UnknownData {};
  struct TextData {};
  struct PasswordData {};
  struct CheckboxData {
    QString label;
  };
  struct AppPickerData {};
  struct FilePickerData {};
  struct DirectoryPickerData {};
  struct DropdownData {
    struct Option {
      QString title;
      QString value;
    };

    std::vector<Option> options;
  };

private:
  using Data = std::variant<UnknownData, TextData, PasswordData, CheckboxData, DropdownData>;
  QString m_name;
  QString m_title;
  QString m_description;
  QString m_placeholder;
  QJsonValue m_value;
  bool m_required;
  Data m_data = UnknownData();

  Preference(const Data &data) : m_data(data) {}

public:
  static Preference makeCheckbox(const QString &label = "") { return {CheckboxData{.label = label}}; }
  static Preference makeText() { return {TextData{}}; }
  static Preference makePassword() { return {PasswordData{}}; }
  static Preference makeDropdown(const std::vector<DropdownData::Option> &options = {}) {
    return {DropdownData{options}};
  }

  void setName(const QString &name) { m_name = name; }
  void setTitle(const QString &name) { m_title = name; }
  void setDescription(const QString &name) { m_description = name; }
  void setPlaceholder(const QString &name) { m_placeholder = name; }
  void setRequired(bool required) { m_required = required; }
  void setData(const Data &data) { m_data = data; }
  void setDefaultValue(const QJsonValue &value) { m_value = value; }
  bool isValid() const { return !std::holds_alternative<UnknownData>(m_data); }

  QString name() const { return m_name; }
  QString title() const { return m_title; }
  QString description() const { return m_description; }
  QString placeholder() const { return m_placeholder; }
  QJsonValue defaultValue() const { return m_value; }
  bool required() const { return m_required; }
  Data data() const { return m_data; }

  Preference() {}
};

using PreferenceList = std::vector<Preference>;
