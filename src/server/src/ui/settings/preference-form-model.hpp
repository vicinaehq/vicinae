#pragma once
#include <QtQml/qqmlregistration.h>
#include "ui/quick/completion-model.hpp"
#include "ui/views/app-selector-model.hpp"
#include "command/preference.hpp"
#include "common/entrypoint.hpp"
#include <QAbstractListModel>
#include <QTimer>
#include <QUrl>
#include <vector>

class PreferenceFormModel : public QAbstractListModel {
  Q_OBJECT
  QML_NAMED_ELEMENT(PreferenceFormModel)
  QML_UNCREATABLE("")

public:
  enum Role {
    TypeRole = Qt::UserRole + 1,
    FieldIdRole,
    LabelRole,
    CheckboxLabelRole,
    DescriptionRole,
    PlaceholderRole,
    ValueRole,
    DropdownModelRole,
    CurrentDropdownItemRole,
    ReadOnlyRole,
    MultipleRole,
    CanChooseFilesRole,
    CanChooseDirectoriesRole,
    LockedPathsRole,
    ComponentRole
  };

  explicit PreferenceFormModel(QObject *parent = nullptr);
  ~PreferenceFormModel() override;

  int rowCount(const QModelIndex &parent = {}) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QHash<int, QByteArray> roleNames() const override;

  void load(const EntrypointId &id, const std::vector<Preference> &preferences);
  void loadProvider(const QString &providerId, const std::vector<Preference> &preferences);

  Q_INVOKABLE void setFieldValue(int row, const QVariant &value);
  Q_INVOKABLE QUrl componentFor(const QString &fieldId) const;

private:
  void save();

  struct Field {
    QString type;
    QString id;
    QString label;
    QString checkboxLabel;
    QString description;
    QString placeholder;
    QVariant value;
    CompletionModel *dropdownModel = nullptr;
    bool readOnly = false;
    bool multiple = false;
    bool canChooseFiles = true;
    bool canChooseDirectories = false;
    QStringList lockedPaths;
    QUrl component;
  };

  Field createField(const Preference &pref);
  void clearFields();
  CompletionModel *appModel();
  static QVariant currentDropdownItem(const Field &f);

  std::vector<Field> m_fields;
  AppSelectorModel *m_appModel = nullptr;
  PreferenceValues m_values;
  EntrypointId m_itemId;
  QString m_providerId;
  bool m_isProvider = false;
  QTimer m_saveTimer;
};
