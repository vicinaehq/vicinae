#pragma once
#include "extend/action-model.hpp"
#include "extend/event-counted.hpp"
#include "extend/list-model.hpp"
#include "extend/model.hpp"
#include <optional>
#include <qjsonvalue.h>
#include <qstring.h>
#include <variant>

struct FormModel {
  struct FieldBase {
    QString id;
    bool autoFocus;
    std::optional<QJsonValue> defaultValue;
    std::optional<QString> error;
    std::optional<QString> info;
    std::optional<EventHandler> onBlur;
    std::optional<EventHandler> onChange;
    std::optional<EventHandler> onFocus;
    std::optional<QString> title;
    std::optional<EventCounted<QJsonValue>> value;
    bool storeValue;
  };

  struct TextField {
    FieldBase base;
    std::optional<QString> placeholder;
  };

  struct PasswordField {
    FieldBase base;
    std::optional<QString> placeholder;
  };

  struct CheckboxField {
    FieldBase base;
    std::optional<QString> label;
  };

  struct DropdownField {
    FieldBase base;
    std::vector<DropdownModel::Child> items;
    std::optional<QString> onSearchTextChange;
    std::optional<QString> placeholder;
    bool isLoading;
    bool throttle;
    std::optional<QString> tooltip;
    bool filtering;
  };

  struct TextAreaField {
    FieldBase base;
    std::optional<QString> placeholder;
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
    std::optional<QString> min;
    std::optional<QString> max;
    std::optional<QString> type;
  };

  struct Separator {};
  struct Description {
    QString text;
    std::optional<QString> title;
  };

  struct LinkAccessoryModel {
    QString text;
    QString target;
  };

  using Field = std::variant<TextField, PasswordField, CheckboxField, DropdownField, TextAreaField,
                             FilePickerField, DatePickerField>;
  using FormSearchBarAccessory = std::variant<DropdownModel, LinkAccessoryModel>;
  using Item = std::variant<Field, Description, Separator>;

  bool isLoading;
  bool enableDrafts;
  std::optional<QString> navigationTitle;
  std::optional<FormSearchBarAccessory> searchBarAccessory;
  std::optional<ActionPannelModel> actions;
  std::vector<Item> items;
};
