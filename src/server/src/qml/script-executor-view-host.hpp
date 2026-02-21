#pragma once
#include "bridge-view.hpp"
#include <QTimer>
#include <optional>

class ScriptProcess;

class ScriptExecutorViewHost : public FormViewBase {
  Q_OBJECT
  Q_PROPERTY(QString outputHtml READ outputHtml NOTIFY outputChanged)
  Q_PROPERTY(bool running READ running NOTIFY runningChanged)

signals:
  void outputChanged();
  void runningChanged();

public:
  explicit ScriptExecutorViewHost(ScriptProcess *process);

  QUrl qmlComponentUrl() const override;
  QVariantMap qmlProperties() const override;
  void initialize() override;
  void beforePop() override;

  QString outputHtml() const { return m_html; }
  bool running() const { return !m_exited; }

  Q_INVOKABLE void killProcess();
  Q_INVOKABLE void rerun();

private:
  void startProcess();
  void rebuildHtml();
  void generateActions();

  ScriptProcess *m_process;
  QString m_rawOutput;
  QString m_html;
  QTimer m_toastUpdater;
  std::optional<QDateTime> m_startedAt;
  bool m_exited = false;
};
