#pragma once
#include <glaze/json/generic.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
#include <qnamespace.h>
#include <qstring.h>
#include "ui/image/url.hpp"

using PreferenceValues = glz::generic::object_t;

namespace preferences {

inline const glz::generic *find(const PreferenceValues &values, std::string_view key) {
  auto it = values.find(std::string(key));
  return it != values.end() ? &it->second : nullptr;
}

inline bool isEmpty(const glz::generic &value) {
  if (value.is_null()) return true;
  if (value.is_string()) return value.get_string().empty();
  if (value.is_array()) return value.get_array().empty();
  return false;
}

} // namespace preferences

class Preference {
public:
  struct UnknownData {};
  struct TextData {};
  struct PasswordData {};
  struct CheckboxData {
    std::optional<QString> label;
  };
  struct AppPickerData {
    bool multiple = false;
  };
  struct ShortcutData {};
  struct CustomData {
    QString component;
  };
  struct FilePickerData {
    bool multiple = false;
    // Paths that are always part of the effective set but are not part of the stored
    // value: they are displayed as non-removable entries.
    std::vector<QString> lockedPaths;
  };
  struct DirectoryPickerData {
    bool multiple = false;
    std::vector<QString> lockedPaths;
  };
  struct DropdownData {
    struct Option {
      QString title;
      QString value;
      std::optional<ImageURL> icon;
    };

    struct Section {
      QString title;
      std::vector<Option> options;
    };

    std::vector<Option> options;
    std::vector<Section> sections;
  };

private:
  using Data = std::variant<UnknownData, TextData, PasswordData, CheckboxData, DropdownData, FilePickerData,
                            DirectoryPickerData, AppPickerData, ShortcutData, CustomData>;
  QString m_name;
  QString m_title;
  QString m_description;
  QString m_placeholder;
  std::optional<glz::generic> m_default;
  bool m_readOnly = false;
  bool m_required = true;
  Data m_data = UnknownData();

  Preference(const QString &id, const Data &data) : m_name(id), m_data(data) {}

public:
  static Preference makeCheckbox(const QString &id, const std::optional<QString> &label = {}) {
    return {id, CheckboxData{.label = label}};
  }
  static Preference makeText(const QString &id) { return {id, TextData{}}; }
  static Preference makePassword(const QString &id) { return {id, PasswordData{}}; }
  static Preference makeShortcut(const QString &id) { return {id, ShortcutData{}}; }
  static Preference custom(const QString &id, const QString &component) {
    return {id, CustomData{.component = component}};
  }
  static Preference makeDropdown(const QString &id, const std::vector<DropdownData::Option> &options = {}) {
    return {id, DropdownData{.options = options}};
  }
  static Preference makeDropdown(const QString &id, const std::vector<DropdownData::Section> &sections) {
    return {id, DropdownData{.sections = sections}};
  }
  static Preference file(const QString &id) { return {id, FilePickerData()}; }
  static Preference files(const QString &id) { return {id, FilePickerData{.multiple = true}}; }
  static Preference app(const QString &id) { return {id, AppPickerData{}}; }
  static Preference apps(const QString &id) {
    Preference preference{id, AppPickerData{.multiple = true}};
    preference.setDefaultValue(glz::generic::array_t{});
    return preference;
  }
  static Preference directory(const QString &id) { return {id, DirectoryPickerData{}}; }
  static Preference directories(const QString &id, std::vector<QString> lockedPaths = {}) {
    Preference preference{id, DirectoryPickerData{.multiple = true, .lockedPaths = std::move(lockedPaths)}};
    preference.setDefaultValue(glz::generic::array_t{});
    return preference;
  }

  void setName(const QString &name) { m_name = name; }
  void setTitle(const QString &name) { m_title = name; }
  void setDescription(const QString &name) { m_description = name; }
  void setPlaceholder(const QString &name) { m_placeholder = name; }
  void setRequired(bool required) { m_required = required; }
  void setReadOnly(bool value = true) { m_readOnly = value; }
  void setData(const Data &data) { m_data = data; }
  void setDefaultValue(glz::generic value) { m_default = std::move(value); }
  void setDefaultValue(const char *value) { m_default = std::string(value); }
  void setDefaultValue(const QString &value) { m_default = value.toStdString(); }
  bool hasDefaultValue() const { return m_default.has_value(); }
  bool isValid() const { return !std::holds_alternative<UnknownData>(m_data); }
  bool isReadOnly() const { return m_readOnly; }

  QString name() const { return m_name; }
  QString title() const { return m_title; }
  QString description() const { return m_description; }
  QString placeholder() const { return m_placeholder; }
  const std::optional<glz::generic> &defaultValue() const { return m_default; }
  glz::generic defaultOrNull() const { return m_default.value_or(glz::generic{}); }
  bool required() const { return m_required; }
  Data data() const { return m_data; }
  bool isSecret() const { return std::holds_alternative<PasswordData>(m_data); }

  Preference() = default;
};

using PreferenceList = std::vector<Preference>;
