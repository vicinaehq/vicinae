#pragma once
#include "extend/form-model.hpp"
#include <QAbstractListModel>
#include <QJsonValue>
#include <QVariantMap>
#include <expected>
#include <functional>
#include <vector>

class QmlExtensionFormModel : public QAbstractListModel {
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

  explicit QmlExtensionFormModel(NotifyFn notify, QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  Q_INVOKABLE void setFieldValue(int index, const QVariant &value);
  Q_INVOKABLE void fieldFocused(int index);
  Q_INVOKABLE void fieldBlurred(int index);
  Q_INVOKABLE void setFilePaths(int index, const QVariantList &paths);
  Q_INVOKABLE void dropdownSearchTextChanged(int index, const QString &text);

  void setFormData(const FormModel &model);
  std::expected<QJsonObject, QString> submit() const;

private:
  struct FormItemData {
    enum class Type { Text, Password, Checkbox, Dropdown, TextArea, FilePicker, DatePicker, Description, Separator };

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

    std::optional<EventHandler> onChange;
    std::optional<EventHandler> onBlur;
    std::optional<EventHandler> onFocus;

    QVariantMap fieldData;

    QJsonValue effectiveValue() const { return hasUserValue ? userValue : modelValue; }
    QString typeString() const;
    bool isField() const { return type != Type::Description && type != Type::Separator; }
  };

  FormItemData createItem(const FormModel::Item &item) const;
  void updateItem(FormItemData &existing, const FormModel::Item &newItem);

  static FormItemData::Type fieldType(const FormModel::IField &field);
  static QVariantMap buildFieldData(const FormModel::IField &field);

  NotifyFn m_notify;
  std::vector<FormItemData> m_items;
  bool m_firstLoad = true;
};
