#pragma once
#include "preference.hpp"
#include "common/entrypoint.hpp"
#include <QAbstractListModel>
#include <QJsonObject>
#include <QTimer>
#include <vector>

class PreferenceFormModel : public QAbstractListModel {
  Q_OBJECT

signals:
  void filePickerResult(int index, const QVariantList &paths);
  void openQmlFilePicker(int index);

public:
  enum Role {
    TypeRole = Qt::UserRole + 1,
    FieldIdRole,
    LabelRole,
    DescriptionRole,
    PlaceholderRole,
    ValueRole,
    OptionsRole,
    ReadOnlyRole,
    MultipleRole,
    CanChooseFilesRole,
    CanChooseDirectoriesRole
  };

  explicit PreferenceFormModel(QObject *parent = nullptr);
  ~PreferenceFormModel() override;

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void load(const EntrypointId &id, const std::vector<Preference> &preferences);
  void loadProvider(const QString &providerId, const std::vector<Preference> &preferences);

  Q_INVOKABLE void setFieldValue(int row, const QVariant &value);
  Q_INVOKABLE void openFilePicker(int row);
  Q_INVOKABLE void closeFallbackDialog();

private:
  void save();

  struct Field {
    QString type;
    QString id;
    QString label;
    QString description;
    QString placeholder;
    QVariant value;
    QVariantList options;
    bool readOnly = false;
    bool multiple = false;
    bool canChooseFiles = true;
    bool canChooseDirectories = false;
  };

  std::vector<Field> m_fields;
  QJsonObject m_values;
  EntrypointId m_itemId;
  QString m_providerId;
  bool m_isProvider = false;
  QTimer m_saveTimer;
};
