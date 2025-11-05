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

  struct CheckboxField : public IField {
    std::optional<QString> m_label;

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

  struct TextAreaField : public IField {
    std::optional<QString> placeholder;

  public:
    TextAreaField(const FieldBase &base) : IField(base) {}
  };

  struct FilePickerField : public IField {
    bool allowMultipleSelection = false;
    bool canChooseDirectories = false;
    bool canChooseFiles = true;
    bool showHiddenFiles = false;

  public:
    FilePickerField(const FieldBase &base) : IField(base) {}
  };

  struct DatePickerField : public IField {
    std::optional<QString> min;  // ISO string
    std::optional<QString> max;  // ISO string
    std::optional<QString> type; // "date" | "dateTime"

  public:
    DatePickerField(const FieldBase &base) : IField(base) {}
  };
  struct InvalidField : public FieldBase {};

  struct Separator {};
  struct Description {
    QString text;
    std::optional<QString> title;
  };

  struct LinkAccessoryModel {
    QString text;
    QString target;
  };

  // note: only LinkAccessoryModel is possible right now
  using FormSearchBarAccessory = std::variant<DropdownModel, LinkAccessoryModel>;
  using Item = std::variant<std::shared_ptr<IField>, Description, Separator>;

  bool isLoading;
  bool enableDrafts;
  std::optional<QString> navigationTitle;
  std::optional<FormSearchBarAccessory> searchBarAccessory;
  std::optional<ActionPannelModel> actions;
  std::vector<Item> items;

  static FormModel fromJson(const QJsonObject &json);
};
