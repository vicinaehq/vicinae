#pragma once
#include <optional>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>
#include "services/ai-commands/ai-command-store.hpp"
#include "ui/quick/completion-model.hpp"
#include "ui/views/bridge-view.hpp"

class AiCommandFormViewHost : public FormViewBase {
  Q_OBJECT
  QML_NAMED_ELEMENT(AiCommandFormViewHost)
  QML_UNCREATABLE("")

  Q_PROPERTY(QString name READ name WRITE setName NOTIFY formChanged)
  Q_PROPERTY(QString prompt READ prompt WRITE setPrompt NOTIFY formChanged)
  Q_PROPERTY(QVariantList promptCompletions READ promptCompletions CONSTANT)
  Q_PROPERTY(QVariantMap selectedIcon READ selectedIcon NOTIFY formChanged)
  Q_PROPERTY(CompletionModel *iconModel READ iconModel CONSTANT)
  Q_PROPERTY(QVariantList modelItems READ modelItems NOTIFY modelsChanged)
  Q_PROPERTY(QVariantMap currentModel READ currentModel NOTIFY modelsChanged)
  Q_PROPERTY(QVariantList outputItems READ outputItems CONSTANT)
  Q_PROPERTY(QVariantMap currentOutput READ currentOutput NOTIFY formChanged)
  Q_PROPERTY(QString nameError READ nameError NOTIFY errorsChanged)
  Q_PROPERTY(QString promptError READ promptError NOTIFY errorsChanged)
  Q_PROPERTY(QString modelError READ modelError NOTIFY errorsChanged)

public:
  Q_INVOKABLE void submit();
  Q_INVOKABLE void selectModel(const QVariantMap &item);
  Q_INVOKABLE void selectIcon(const QVariantMap &item);
  Q_INVOKABLE void selectOutput(const QVariantMap &item);

signals:
  void formChanged();
  void modelsChanged();
  void errorsChanged();

public:
  enum class Mode { Create, Edit, Duplicate };

  AiCommandFormViewHost() = default;
  AiCommandFormViewHost(AiCommandStore::Command command, Mode mode);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() override;
  QString initialNavigationTitle() const override;
  ImageURL initialNavigationIcon() const override;
  void initialize() override;

  QString name() const { return m_name; }
  QString prompt() const { return m_prompt; }
  QVariantList promptCompletions() const;
  QVariantMap selectedIcon() const { return m_selectedIcon; }
  CompletionModel *iconModel() { return &m_iconModel; }
  QVariantList modelItems() const { return m_modelItems; }
  QVariantMap currentModel() const { return m_currentModel; }
  QVariantList outputItems() const;
  QVariantMap currentOutput() const;
  QString nameError() const { return m_nameError; }
  QString promptError() const { return m_promptError; }
  QString modelError() const { return m_modelError; }

  void setName(const QString &value);
  void setPrompt(const QString &value);

private:
  void buildIconItems();
  void rebuildModels();
  const AiCommandStore::Command *editedCommand() const;

  Mode m_mode = Mode::Create;
  std::optional<AiCommandStore::Command> m_initial;
  QString m_name;
  QString m_prompt;
  QString m_modelId;
  AiCommandStore::Output m_output = AiCommandStore::Output::Paste;
  QVariantMap m_selectedIcon;
  QVariantMap m_defaultIconEntry;
  CompletionModel m_iconModel{this};
  QVariantList m_modelItems;
  QVariantMap m_currentModel;
  QString m_nameError;
  QString m_promptError;
  QString m_modelError;
};
