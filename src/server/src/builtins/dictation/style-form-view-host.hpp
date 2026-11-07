#pragma once
#include <optional>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include "services/dictation/dictation-styles.hpp"
#include "ui/views/app-selector-model.hpp"
#include "ui/views/bridge-view.hpp"

class DictationStyleFormViewHost : public FormViewBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(DictationStyleFormViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY formChanged)
  Q_PROPERTY(QString instructions READ instructions WRITE setInstructions NOTIFY formChanged)
  Q_PROPERTY(QStringList apps READ apps WRITE setApps NOTIFY formChanged)
  Q_PROPERTY(bool useAsDefault READ useAsDefault WRITE setUseAsDefault NOTIFY formChanged)
  Q_PROPERTY(QVariantList modelItems READ modelItems NOTIFY modelsChanged)
  Q_PROPERTY(QVariantMap currentModel READ currentModel NOTIFY modelsChanged)
  Q_PROPERTY(AppSelectorModel *appSelectorModel READ appSelectorModel CONSTANT)
  Q_PROPERTY(QString nameError READ nameError NOTIFY errorsChanged)
  Q_PROPERTY(QString instructionsError READ instructionsError NOTIFY errorsChanged)
  Q_PROPERTY(QString modelError READ modelError NOTIFY errorsChanged)
  Q_PROPERTY(QString appsError READ appsError NOTIFY errorsChanged)

public:
  Q_INVOKABLE void submit();
  Q_INVOKABLE void selectModel(const QVariantMap &item);

signals:
  void formChanged();
  void modelsChanged();
  void errorsChanged();

public:
  DictationStyleFormViewHost() = default;
  explicit DictationStyleFormViewHost(DictationStyles::Style style);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  QString initialNavigationTitle() const override;
  ImageURL initialNavigationIcon() const override;
  void initialize() override;

  QString name() const { return m_name; }
  QString instructions() const { return m_instructions; }
  QStringList apps() const { return m_apps; }
  bool useAsDefault() const { return m_useAsDefault; }
  QVariantList modelItems() const { return m_modelItems; }
  QVariantMap currentModel() const { return m_currentModel; }
  AppSelectorModel *appSelectorModel() const { return m_appSelectorModel; }
  QString nameError() const { return m_nameError; }
  QString instructionsError() const { return m_instructionsError; }
  QString modelError() const { return m_modelError; }
  QString appsError() const { return m_appsError; }

  void setName(const QString &value);
  void setInstructions(const QString &value);
  void setApps(const QStringList &value);
  void setUseAsDefault(bool value);

private:
  void rebuildModels();
  QString appsConflict() const;

  std::optional<DictationStyles::Style> m_initial;
  QString m_name;
  QString m_instructions;
  QString m_modelId;
  QStringList m_apps;
  bool m_useAsDefault = false;
  QVariantList m_modelItems;
  QVariantMap m_currentModel;
  AppSelectorModel *m_appSelectorModel = nullptr;
  QString m_nameError;
  QString m_instructionsError;
  QString m_modelError;
  QString m_appsError;
};
