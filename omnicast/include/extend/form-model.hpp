#pragma once
#include "extend/action-model.hpp"
#include "extend/list-model.hpp"
#include "extend/model.hpp"
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qstring.h>
#include <optional>

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
    std::optional<QString> placeholder;
    std::optional<QString> title;
    std::optional<QJsonValue> value;
    bool storeValue;
  };

  struct TextFieldModel : public FieldBase {};
  struct PasswordFieldModel : public FieldBase {};
  struct TextAreaFieldModel : public FieldBase {
    bool enableMarkdown;
  };
  struct CheckboxFieldModel : public FieldBase {};
  struct DatePickerFieldModel : public FieldBase {};
  struct InvalidField : public FieldBase {};

  struct Separator {};
  struct Description {
    QString text;
    std::optional<QString> title;
  };

  using SearchAccessory = std::variant<DropdownModel>;
  using Field = std::variant<TextFieldModel, PasswordFieldModel, TextAreaFieldModel, CheckboxFieldModel,
                             DatePickerFieldModel, InvalidField>;
  using Item = std::variant<Field, Description, Separator>;

  bool isLoading;
  bool enableDrafts;
  std::optional<QString> navigationTitle;
  std::optional<SearchAccessory> accessory;
  std::optional<ActionPannelModel> actions;
  std::vector<Item> items;

  static FormModel fromJson(const QJsonObject &json);
};
