#pragma once
#include "extend/action-model.hpp"
#include "extend/list-model.hpp"
#include "extend/model.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qcoreevent.h>
#include <qjsonobject.h>
#include <qjsonvalue.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstring.h>
#include <optional>
#include <qtmetamacros.h>
#include <qwidget.h>

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

  class IField : public FieldBase {
  public:
    size_t fieldTypeId() const { return typeid(*this).hash_code(); }

    virtual ~IField() {}

    IField(const FieldBase &base) : FieldBase(base) {}
  };

  struct TextField : public IField {
    std::optional<QString> m_placeholder;

  public:
    TextField(const FieldBase &base) : IField(base) {}
  };

  struct PasswordField : public IField {
    std::optional<QString> m_placeholder;

  public:
    PasswordField(const FieldBase &base) : IField(base) {}
  };

  class CheckboxField : public IField {
  public:
    CheckboxField(const FieldBase &base) : IField(base) {}
  };

  struct DropdownField : public IField {
    std::vector<DropdownModel::Child> m_items;
    std::optional<QString> onSearchTextChange;
    std::optional<QString> placeholder;
    bool isLoading;
    bool throttle;
    std::optional<QString> tooltip;
    bool filtering;

    DropdownField(const FieldBase &base) : IField(base) {}
  };

  struct TextAreaFieldModel : public FieldBase {
    bool enableMarkdown;
  };
  struct DatePickerFieldModel : public FieldBase {};
  struct InvalidField : public FieldBase {};

  struct Separator {};
  struct Description {
    QString text;
    std::optional<QString> title;
  };

  using SearchAccessory = std::variant<DropdownModel>;
  using Item = std::variant<std::shared_ptr<IField>, Description, Separator>;

  bool isLoading;
  bool enableDrafts;
  std::optional<QString> navigationTitle;
  std::optional<SearchAccessory> accessory;
  std::optional<ActionPannelModel> actions;
  std::vector<Item> items;

  static FormModel fromJson(const QJsonObject &json);
};
