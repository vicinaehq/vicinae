#pragma once
#include "common/entrypoint.hpp"
#include "preference.hpp"
#include "qml-bridge-view.hpp"
#include <QAbstractListModel>
#include <QJsonObject>
#include <memory>
#include <vector>

class ExtensionCommand;

class QmlMissingPreferenceFormModel : public QAbstractListModel {
  Q_OBJECT

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
    DirectoriesOnlyRole
  };

  explicit QmlMissingPreferenceFormModel(QObject *parent = nullptr);

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void load(const std::vector<Preference> &preferences, const QJsonObject &existingValues);
  Q_INVOKABLE void setFieldValue(int row, const QVariant &value);

  QJsonObject values() const { return m_values; }

  struct ValidateResult {
    bool valid;
    int firstInvalidRow;
  };

  ValidateResult validate() const;

signals:
  void validationChanged();

private:
  struct Field {
    QString type;
    QString id;
    QString label;
    QString description;
    QString placeholder;
    QVariant value;
    QVariantList options;
    bool multiple = false;
    bool directoriesOnly = false;
  };

  std::vector<Field> m_fields;
  QJsonObject m_values;
};

class QmlMissingPreferenceViewHost : public QmlFormViewBase {
  Q_OBJECT

  Q_PROPERTY(QString commandName READ commandName CONSTANT)
  Q_PROPERTY(QString commandIconSource READ commandIconSource CONSTANT)
  Q_PROPERTY(QmlMissingPreferenceFormModel *prefModel READ prefModel CONSTANT)

public:
  QmlMissingPreferenceViewHost(std::shared_ptr<ExtensionCommand> command,
                               const std::vector<Preference> &preferences,
                               const QJsonObject &preferenceValues);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;

  Q_INVOKABLE void submit();

  QString commandName() const;
  QString commandIconSource() const;
  QmlMissingPreferenceFormModel *prefModel() const { return m_prefModel; }

private:
  std::shared_ptr<ExtensionCommand> m_command;
  QmlMissingPreferenceFormModel *m_prefModel;
  QString m_commandIconSource;
};
