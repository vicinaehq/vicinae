#include "extension-form-model.hpp"
#include "view-utils.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include <utility>

template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};

static inline QString toQ(const std::string &s) {
  return QString::fromUtf8(s.data(), static_cast<qsizetype>(s.size()));
}

static inline QString optToQ(const std::optional<std::string> &s) { return s ? toQ(*s) : QString(); }

ExtensionFormModel::ExtensionFormModel(NotifyFn notify, QObject *parent)
    : QAbstractListModel(parent), m_notify(std::move(notify)) {}

int ExtensionFormModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return static_cast<int>(m_items.size());
}

QVariant ExtensionFormModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_items.size())) return {};

  const auto &item = m_items[index.row()];

  switch (role) {
  case TypeRole:
    return item.typeString();
  case FieldIdRole:
    return item.fieldId;
  case LabelRole:
    return item.label;
  case ErrorRole:
    return item.error;
  case InfoRole:
    return item.info;
  case PlaceholderRole:
    return item.placeholder;
  case ValueRole:
    return item.effectiveValue().toVariant();
  case AutoFocusRole:
    return item.autoFocus;
  case FieldDataRole:
    return item.fieldData;
  default:
    return {};
  }
}

QHash<int, QByteArray> ExtensionFormModel::roleNames() const {
  return {
      {TypeRole, "type"},   {FieldIdRole, "fieldId"},     {LabelRole, "label"},
      {ErrorRole, "error"}, {InfoRole, "info"},           {PlaceholderRole, "placeholder"},
      {ValueRole, "value"}, {AutoFocusRole, "autoFocus"}, {FieldDataRole, "fieldData"},
  };
}

void ExtensionFormModel::setFieldValue(int index, const QVariant &value) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  auto &item = m_items[index];
  if (!item.isField()) return;

  item.userValue = QJsonValue::fromVariant(value);
  item.hasUserValue = true;
  ++item.eventCount;
  emit dataChanged(createIndex(index, 0), createIndex(index, 0), {ValueRole});

  if (item.onChange) {
    m_notify(toQ(*item.onChange), QJsonArray{item.userValue, static_cast<int>(item.eventCount)});
  }
}

void ExtensionFormModel::fieldFocused(int index) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  const auto &item = m_items[index];
  if (item.onFocus) { m_notify(toQ(*item.onFocus), {}); }
}

void ExtensionFormModel::fieldBlurred(int index) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  const auto &item = m_items[index];
  if (item.onBlur) { m_notify(toQ(*item.onBlur), {}); }
}

bool ExtensionFormModel::isExtensionControlled(int index) const {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return false;
  const auto &v = m_items[index].modelValue;
  return !v.isUndefined() && !v.isNull();
}

void ExtensionFormModel::setFilePaths(int index, const QVariantList &paths) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  auto &item = m_items[index];
  if (item.type != FormItemData::Type::FilePicker) return;

  QJsonArray arr;
  for (const auto &p : paths) {
    arr.append(p.toString());
  }

  bool const extensionControlled = !item.modelValue.isUndefined() && !item.modelValue.isNull();
  if (!extensionControlled) {
    item.userValue = arr;
    item.hasUserValue = true;
    emit dataChanged(createIndex(index, 0), createIndex(index, 0), {ValueRole});
  }

  if (item.onChange) { m_notify(toQ(*item.onChange), QJsonArray{arr}); }
}

void ExtensionFormModel::dropdownSearchTextChanged(int index, const QString &text) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  const auto &item = m_items[index];
  auto handler = item.fieldData.value("onSearchTextChange").toString();
  if (!handler.isEmpty()) { m_notify(handler, QJsonArray{text}); }
}

std::optional<FileChooserOptions> ExtensionFormModel::filePickerOptions(int index) const {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return std::nullopt;
  const auto &item = m_items[index];
  if (item.type != FormItemData::Type::FilePicker) return std::nullopt;

  FileChooserOptions opts;
  opts.canChooseFiles = item.fieldData.value("canChooseFiles", true).toBool();
  opts.canChooseDirectories = item.fieldData.value("canChooseDirectories", false).toBool();
  opts.allowMultipleSelection = item.fieldData.value("multiple", false).toBool();
  opts.showHiddenFiles = item.fieldData.value("showHidden", false).toBool();
  return opts;
}

void ExtensionFormModel::setFormData(const FormModel &model) {
  std::unordered_map<QString, QJsonValue> savedValues;
  for (const auto &item : m_items) {
    if (item.isField() && item.hasUserValue) { savedValues[item.fieldId] = item.userValue; }
  }

  int const oldCount = static_cast<int>(m_items.size());
  int const newCount = static_cast<int>(model.items.size());

  if (oldCount > newCount) {
    beginRemoveRows({}, newCount, oldCount - 1);
    m_items.resize(newCount);
    endRemoveRows();
  }

  int const updateCount = std::min(oldCount, newCount);
  for (int i = 0; i < updateCount; ++i) {
    updateItem(m_items[i], model.items[i]);

    auto newData = createItem(model.items[i]);
    if (m_items[i].type != newData.type || m_items[i].fieldId != newData.fieldId) {
      if (newData.isField() && !newData.hasUserValue) {
        auto it = savedValues.find(newData.fieldId);
        if (it != savedValues.end()) {
          newData.userValue = it->second;
          newData.hasUserValue = true;
        }
      }
      m_items[i] = std::move(newData);
    }
  }
  if (updateCount > 0) { emit dataChanged(createIndex(0, 0), createIndex(updateCount - 1, 0)); }

  if (newCount > oldCount) {
    beginInsertRows({}, oldCount, newCount - 1);
    for (int i = oldCount; i < newCount; ++i) {
      auto newData = createItem(model.items[i]);
      if (newData.isField() && !newData.hasUserValue) {
        auto it = savedValues.find(newData.fieldId);
        if (it != savedValues.end()) {
          newData.userValue = it->second;
          newData.hasUserValue = true;
        }
      }
      m_items.push_back(std::move(newData));
    }
    endInsertRows();
  }

  if (m_firstLoad) {
    m_firstLoad = false;
    for (int i = 0; std::cmp_less(i, m_items.size()); ++i) {
      if (m_items[i].autoFocus) {
        emit autoFocusRequested(i);
        break;
      }
    }
  }
}

std::expected<QJsonObject, QString> ExtensionFormModel::submit() const {
  QJsonObject payload;
  for (const auto &item : m_items) {
    if (!item.isField()) continue;
    if (!item.storeValue) continue;
    if (!item.error.isEmpty()) return std::unexpected(QString("One or more fields have errors"));
    payload[item.fieldId] = item.effectiveValue();
  }
  return payload;
}

const FormModel::FieldBase &ExtensionFormModel::getBase(const FormModel::Field &field) {
  return std::visit([](const auto &f) -> const FormModel::FieldBase & { return f.base; }, field);
}

ExtensionFormModel::FormItemData ExtensionFormModel::createItem(const FormModel::Item &item) const {
  FormItemData data{};

  if (auto *fieldPtr = std::get_if<FormModel::Field>(&item)) {
    const auto &base = getBase(*fieldPtr);
    data.type = fieldType(*fieldPtr);
    data.fieldId = toQ(base.id);
    data.label = optToQ(base.title);
    data.error = optToQ(base.error);
    data.info = optToQ(base.info);
    data.autoFocus = base.autoFocus;
    data.storeValue = base.storeValue;
    data.onChange = base.onChange;
    data.onBlur = base.onBlur;
    data.onFocus = base.onFocus;
    data.fieldData = buildFieldData(*fieldPtr);

    data.placeholder = std::visit(overloaded{
                                      [](const FormModel::TextField &f) { return optToQ(f.placeholder); },
                                      [](const FormModel::PasswordField &f) { return optToQ(f.placeholder); },
                                      [](const FormModel::DropdownField &f) { return optToQ(f.placeholder); },
                                      [](const FormModel::TextAreaField &f) { return optToQ(f.placeholder); },
                                      [](const auto &) { return QString(); },
                                  },
                                  *fieldPtr);

    if (base.value) {
      data.modelValue = base.value->value;
    } else if (base.defaultValue) {
      data.modelValue = *base.defaultValue;
    }
  } else if (auto *desc = std::get_if<FormModel::Description>(&item)) {
    data.type = FormItemData::Type::Description;
    data.label = optToQ(desc->title);
    data.fieldData = {{"text", toQ(desc->text)}};
  } else if (std::holds_alternative<FormModel::Separator>(item)) {
    data.type = FormItemData::Type::Separator;
  }

  return data;
}

void ExtensionFormModel::updateItem(FormItemData &existing, const FormModel::Item &newItem) {
  if (auto *fieldPtr = std::get_if<FormModel::Field>(&newItem)) {
    const auto &base = getBase(*fieldPtr);
    existing.label = optToQ(base.title);
    existing.error = optToQ(base.error);
    existing.info = optToQ(base.info);
    existing.onChange = base.onChange;
    existing.onBlur = base.onBlur;
    existing.onFocus = base.onFocus;
    existing.fieldData = buildFieldData(*fieldPtr);

    existing.placeholder =
        std::visit(overloaded{
                       [](const FormModel::TextField &f) { return optToQ(f.placeholder); },
                       [](const FormModel::PasswordField &f) { return optToQ(f.placeholder); },
                       [](const FormModel::DropdownField &f) { return optToQ(f.placeholder); },
                       [](const FormModel::TextAreaField &f) { return optToQ(f.placeholder); },
                       [](const auto &) { return QString(); },
                   },
                   *fieldPtr);

    if (base.value) {
      if (base.value->eventCount < existing.eventCount) return;
      existing.modelValue = base.value->value;
      existing.hasUserValue = false;
    }
  } else if (auto *desc = std::get_if<FormModel::Description>(&newItem)) {
    existing.label = optToQ(desc->title);
    existing.fieldData = {{"text", toQ(desc->text)}};
  }
}

ExtensionFormModel::FormItemData::Type ExtensionFormModel::fieldType(const FormModel::Field &field) {
  return std::visit(overloaded{
                        [](const FormModel::TextField &) { return FormItemData::Type::Text; },
                        [](const FormModel::PasswordField &) { return FormItemData::Type::Password; },
                        [](const FormModel::CheckboxField &) { return FormItemData::Type::Checkbox; },
                        [](const FormModel::DropdownField &) { return FormItemData::Type::Dropdown; },
                        [](const FormModel::TextAreaField &) { return FormItemData::Type::TextArea; },
                        [](const FormModel::FilePickerField &) { return FormItemData::Type::FilePicker; },
                        [](const FormModel::DatePickerField &) { return FormItemData::Type::DatePicker; },
                    },
                    field);
}

QVariantMap ExtensionFormModel::buildFieldData(const FormModel::Field &field) {
  QVariantMap data;

  std::visit(overloaded{
                 [&](const FormModel::CheckboxField &f) {
                   if (f.label) data["label"] = toQ(*f.label);
                 },
                 [&](const FormModel::DropdownField &f) {
                   data["items"] = qml::convertDropdownChildren(f.items);
                   data["isLoading"] = f.isLoading;
                   data["filtering"] = f.filtering;
                   data["hasRemoteSearch"] = f.onSearchTextChange.has_value();
                   if (f.onSearchTextChange) data["onSearchTextChange"] = toQ(*f.onSearchTextChange);
                   if (f.placeholder) data["placeholder"] = toQ(*f.placeholder);
                   if (f.tooltip) data["tooltip"] = toQ(*f.tooltip);
                 },
                 [&](const FormModel::FilePickerField &f) {
                   data["multiple"] = f.allowMultipleSelection;
                   data["canChooseFiles"] = f.canChooseFiles;
                   data["canChooseDirectories"] = f.canChooseDirectories;
                   data["showHidden"] = f.showHiddenFiles;
                 },
                 [&](const FormModel::DatePickerField &f) {
                   if (f.min) data["min"] = toQ(*f.min);
                   if (f.max) data["max"] = toQ(*f.max);
                   data["includeTime"] = f.type.value_or("date") == "dateTime";
                 },
                 [](const auto &) {},
             },
             field);

  return data;
}

QString ExtensionFormModel::FormItemData::typeString() const {
  switch (type) {
  case Type::Text:
    return QStringLiteral("text");
  case Type::Password:
    return QStringLiteral("password");
  case Type::Checkbox:
    return QStringLiteral("checkbox");
  case Type::Dropdown:
    return QStringLiteral("dropdown");
  case Type::TextArea:
    return QStringLiteral("textarea");
  case Type::FilePicker:
    return QStringLiteral("filepicker");
  case Type::DatePicker:
    return QStringLiteral("datepicker");
  case Type::Description:
    return QStringLiteral("description");
  case Type::Separator:
    return QStringLiteral("separator");
  }
  return QStringLiteral("text");
}
