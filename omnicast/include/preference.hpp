#pragma once
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

using Preference = std::variant<BasePreference, TextFieldPreference, CheckboxPreference, DropdownPreference,
                                PasswordPreference, AppPickerPreference, FilePreference, DirectoryPreference>;
using PreferenceList = std::vector<Preference>;
