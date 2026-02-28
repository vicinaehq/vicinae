#include "extension-form-model.hpp"
#include "view-utils.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include <utility>

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
  emit dataChanged(createIndex(index, 0), createIndex(index, 0), {ValueRole});

  if (item.onChange) { m_notify(*item.onChange, QJsonArray{item.userValue}); }
}

void ExtensionFormModel::fieldFocused(int index) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  const auto &item = m_items[index];
  if (item.onFocus) { m_notify(*item.onFocus, {}); }
}

void ExtensionFormModel::fieldBlurred(int index) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  const auto &item = m_items[index];
  if (item.onBlur) { m_notify(*item.onBlur, {}); }
}

void ExtensionFormModel::setFilePaths(int index, const QVariantList &paths) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  auto &item = m_items[index];
  if (item.type != FormItemData::Type::FilePicker) return;

  QJsonArray arr;
  for (const auto &p : paths) {
    arr.append(p.toString());
  }

  item.userValue = arr;
  item.hasUserValue = true;
  emit dataChanged(createIndex(index, 0), createIndex(index, 0), {ValueRole});

  if (item.onChange) { m_notify(*item.onChange, QJsonArray{arr}); }
}

void ExtensionFormModel::dropdownSearchTextChanged(int index, const QString &text) {
  if (index < 0 || std::cmp_greater_equal(index, m_items.size())) return;
  const auto &item = m_items[index];
  auto handler = item.fieldData.value("onSearchTextChange").toString();
  if (!handler.isEmpty()) { m_notify(handler, QJsonArray{text}); }
}

void ExtensionFormModel::setFormData(const FormModel &model) {
  // Save user values so they survive structural changes
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

ExtensionFormModel::FormItemData ExtensionFormModel::createItem(const FormModel::Item &item) const {
  FormItemData data{};

  if (auto *fieldPtr = std::get_if<std::shared_ptr<FormModel::IField>>(&item)) {
    const auto &field = *fieldPtr;
    data.type = fieldType(*field);
    data.fieldId = field->id;
    data.label = field->title.value_or(QString());
    data.error = field->error.value_or(QString());
    data.info = field->info.value_or(QString());
    data.placeholder = field->placeholder().value_or(QString());
    data.autoFocus = field->autoFocus;
    data.storeValue = field->storeValue;
    data.onChange = field->onChange;
    data.onBlur = field->onBlur;
    data.onFocus = field->onFocus;
    data.fieldData = buildFieldData(*field);

    if (field->value) {
      data.modelValue = *field->value;
    } else if (field->defaultValue) {
      data.modelValue = *field->defaultValue;
    } else if (data.type == FormItemData::Type::Dropdown) {
      if (auto *df = dynamic_cast<const FormModel::DropdownField *>(field.get())) {
        if (auto first = qml::firstDropdownItemValue(df->m_items)) { data.modelValue = *first; }
      }
    }
  } else if (auto *desc = std::get_if<FormModel::Description>(&item)) {
    data.type = FormItemData::Type::Description;
    data.label = desc->title.value_or(QString());
    data.fieldData = {{"text", desc->text}};
  } else if (std::holds_alternative<FormModel::Separator>(item)) {
    data.type = FormItemData::Type::Separator;
  }

  return data;
}

void ExtensionFormModel::updateItem(FormItemData &existing, const FormModel::Item &newItem) {
  if (auto *fieldPtr = std::get_if<std::shared_ptr<FormModel::IField>>(&newItem)) {
    const auto &field = *fieldPtr;
    existing.label = field->title.value_or(QString());
    existing.error = field->error.value_or(QString());
    existing.info = field->info.value_or(QString());
    existing.placeholder = field->placeholder().value_or(QString());
    existing.onChange = field->onChange;
    existing.onBlur = field->onBlur;
    existing.onFocus = field->onFocus;
    existing.fieldData = buildFieldData(*field);

    if (field->value) {
      existing.modelValue = *field->value;
      existing.hasUserValue = false;
    }
  } else if (auto *desc = std::get_if<FormModel::Description>(&newItem)) {
    existing.label = desc->title.value_or(QString());
    existing.fieldData = {{"text", desc->text}};
  }
}

ExtensionFormModel::FormItemData::Type ExtensionFormModel::fieldType(const FormModel::IField &field) {
  if (dynamic_cast<const FormModel::TextField *>(&field)) return FormItemData::Type::Text;
  if (dynamic_cast<const FormModel::PasswordField *>(&field)) return FormItemData::Type::Password;
  if (dynamic_cast<const FormModel::CheckboxField *>(&field)) return FormItemData::Type::Checkbox;
  if (dynamic_cast<const FormModel::DropdownField *>(&field)) return FormItemData::Type::Dropdown;
  if (dynamic_cast<const FormModel::TextAreaField *>(&field)) return FormItemData::Type::TextArea;
  if (dynamic_cast<const FormModel::FilePickerField *>(&field)) return FormItemData::Type::FilePicker;
  if (dynamic_cast<const FormModel::DatePickerField *>(&field)) return FormItemData::Type::DatePicker;
  return FormItemData::Type::Text;
}

QVariantMap ExtensionFormModel::buildFieldData(const FormModel::IField &field) {
  QVariantMap data;

  if (auto *f = dynamic_cast<const FormModel::CheckboxField *>(&field)) {
    if (f->m_label) data["label"] = *f->m_label;
  } else if (auto *f = dynamic_cast<const FormModel::DropdownField *>(&field)) {
    data["items"] = qml::convertDropdownChildren(f->m_items);
    data["isLoading"] = f->isLoading;
    data["filtering"] = f->filtering;
    data["hasRemoteSearch"] = f->onSearchTextChange.has_value();
    if (f->onSearchTextChange) data["onSearchTextChange"] = *f->onSearchTextChange;
    if (f->m_placeholder) data["placeholder"] = *f->m_placeholder;
    if (f->tooltip) data["tooltip"] = *f->tooltip;
  } else if (auto *f = dynamic_cast<const FormModel::FilePickerField *>(&field)) {
    data["multiple"] = f->allowMultipleSelection;
    data["directoriesOnly"] = f->canChooseDirectories && !f->canChooseFiles;
    data["showHidden"] = f->showHiddenFiles;
  } else if (auto *f = dynamic_cast<const FormModel::DatePickerField *>(&field)) {
    if (f->min) data["min"] = *f->min;
    if (f->max) data["max"] = *f->max;
    data["includeTime"] = f->type.value_or("date") == "dateTime";
  }

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
