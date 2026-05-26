#pragma once
#include "extend/action-model.hpp"
#include "extend/event-counted.hpp"
#include "extend/dropdown-model.hpp"
#include "extend/model.hpp"
#include <optional>
#include <qjsonvalue.h>
#include <string>
#include <variant>
#include <vector>

struct FormModel {
  struct FieldBase {
    std::string id;
    bool autoFocus = false;
    std::optional<QJsonValue> defaultValue;
    std::optional<std::string> error;
    std::optional<std::string> info;
    std::optional<EventHandler> onBlur;
    std::optional<EventHandler> onChange;
    std::optional<EventHandler> onFocus;
    std::optional<std::string> title;
    std::optional<EventCounted<QJsonValue>> value;
    bool storeValue = true;
  };

  struct TextField {
    FieldBase base;
    std::optional<std::string> placeholder;
  };

  struct PasswordField {
    FieldBase base;
    std::optional<std::string> placeholder;
  };

  struct CheckboxField {
    FieldBase base;
    std::optional<std::string> label;
  };

  struct DropdownField {
    FieldBase base;
    std::vector<DropdownModel::Child> items;
    std::optional<std::string> onSearchTextChange;
    std::optional<std::string> placeholder;
    bool isLoading = false;
    bool throttle = false;
    std::optional<std::string> tooltip;
    bool filtering = true;
  };

  struct TextAreaField {
    FieldBase base;
    std::optional<std::string> placeholder;
  };

  struct FilePickerField {
    FieldBase base;
    bool allowMultipleSelection = false;
    bool canChooseDirectories = false;
    bool canChooseFiles = true;
    bool showHiddenFiles = false;
  };

  struct DatePickerField {
    FieldBase base;
    std::optional<std::string> min;
    std::optional<std::string> max;
    std::optional<std::string> type;
  };

  struct Separator {};
  struct Description {
    std::string text;
    std::optional<std::string> title;
  };

  struct LinkAccessoryModel {
    std::string text;
    std::string target;
  };

  using Field = std::variant<TextField, PasswordField, CheckboxField, DropdownField, TextAreaField,
                             FilePickerField, DatePickerField>;
  using FormSearchBarAccessory = std::variant<DropdownModel, LinkAccessoryModel>;
  using Item = std::variant<Field, Description, Separator>;

  bool isLoading = false;
  bool enableDrafts = false;
  std::optional<std::string> navigationTitle;
  std::optional<FormSearchBarAccessory> searchBarAccessory;
  std::optional<ActionPannelModel> actions;
  std::vector<Item> items;
};
