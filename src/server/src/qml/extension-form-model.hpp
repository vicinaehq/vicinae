#pragma once
#include "extend/form-model.hpp"
#include "form-tag-picker-suggestions-model.hpp"
#include "services/file-chooser/abstract-file-chooser.hpp"
#include <QAbstractListModel>
#include <QJsonValue>
#include <QVariantMap>
#include <expected>
#include <functional>
#include <optional>
#include <qcontainerfwd.h>
#include <qtmetamacros.h>
#include <vector>

class ExtensionFormModel : public QAbstractListModel {
  Q_OBJECT

signals:
  void autoFocusRequested(int index);

public:
  using NotifyFn = std::function<void(const QString &handler, const QJsonArray &args)>;

  enum Role {
    TypeRole = Qt::UserRole + 1,
    FieldIdRole,
    LabelRole,
    ErrorRole,
    InfoRole,
    PlaceholderRole,
    ValueRole,
    AutoFocusRole,
    FieldDataRole,
  };

  explicit ExtensionFormModel(NotifyFn notify, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void setFieldValue(int index, const QVariant &value);
  Q_INVOKABLE void fieldFocused(int index);
  Q_INVOKABLE void fieldBlurred(int index);
  Q_INVOKABLE void setFilePaths(int index, const QVariantList &paths);
  Q_INVOKABLE void setPickedItems(int index, const QVariantList &pickedItems);
  Q_INVOKABLE void dropdownSearchTextChanged(int index, const QString &text);
  Q_INVOKABLE void tagPickerSearchTextChanged(int index, const QString &text);

  std::optional<FileChooserOptions> filePickerOptions(int index) const;
  bool isExtensionControlled(int index) const;

  void setFormData(const FormModel &model);
  std::expected<QJsonObject, QString> submit() const;

private:
  struct FormItemData {
    enum class Type {
      Text,
      Password,
      Checkbox,
      Dropdown,
      TextArea,
      FilePicker,
      DatePicker,
      TagPicker,
      Description,
      Separator
    };

    Type type;
    QString fieldId;
    QString label;
    QString error;
    QString info;
    QString placeholder;
    bool autoFocus = false;
    bool storeValue = true;

    QJsonValue modelValue;
    QJsonValue userValue;
    bool hasUserValue = false;
    uint32_t eventCount = 0;

    std::optional<EventHandler> onChange;
    std::optional<EventHandler> onBlur;
    std::optional<EventHandler> onFocus;

    std::unique_ptr<FormTagPickerSuggestionsModel> tagPickerSuggestionsModel;
    QVariantMap fieldData;

    QJsonValue effectiveValue() const { return hasUserValue ? userValue : modelValue; }
    QString typeString() const;
    bool isField() const { return type != Type::Description && type != Type::Separator; }
  };

  FormItemData createItem(const FormModel::Item &item) const;
  void updateItem(FormItemData &existing, const FormModel::Item &newItem);
  void updateFieldData(FormItemData &item, const FormModel::IField &field) const;
  void syncTagPickerSuggestionsModel(FormItemData &item) const;
  static QStringList tagPickerPickedItems(const QJsonValue &value);

  static FormItemData::Type fieldType(const FormModel::IField &field);

  NotifyFn m_notify;
  std::vector<FormItemData> m_items;
  bool m_firstLoad = true;
};
