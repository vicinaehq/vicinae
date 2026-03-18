#include "script-executor-view-host.hpp"
#include "ansi-bridge.hpp"
#include "builtin_icon.hpp"
#include "navigation-controller.hpp"
#include "script/script-process.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"

static QString tokenToHtml(const QString &rawOutput) {
  auto html = ansi::to_html(rawOutput.toStdString(), buildAnsiPalette());
  return QStringLiteral("<pre style=\"font-size:10.5pt; white-space:pre-wrap; margin:0;\">") +
         QString::fromStdString(html) + QStringLiteral("</pre>");
}

ScriptExecutorViewHost::ScriptExecutorViewHost(ScriptProcess *process) : m_process(process) {
  process->setParent(this);
}

QUrl ScriptExecutorViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/ScriptExecutorView.qml"));
}

QVariantMap ScriptExecutorViewHost::qmlProperties() {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void ScriptExecutorViewHost::initialize() {
  BaseView::initialize();

  auto toastService = context()->services->toastService();

  connect(m_process, &QProcess::readyReadStandardOutput, this, [this]() {
    m_rawOutput.append(m_process->readAllStandardOutput());
    rebuildHtml();
  });
  connect(m_process, &QProcess::readyReadStandardError, this, [this]() {
    m_rawOutput.append(m_process->readAllStandardError());
    rebuildHtml();
  });
  connect(m_process, &QProcess::errorOccurred, this, [this, toastService]() {
    toastService->failure(QString("Script execution failed: %1").arg(m_process->errorString()));
  });
  connect(m_process, &QProcess::started, this, [this, toastService]() {
    m_startedAt = QDateTime::currentDateTime();
    generateActions();
  });
  connect(&m_toastUpdater, &QTimer::timeout, this, [this, toastService]() {
    if (!m_startedAt || m_exited) return;
    auto secs = m_startedAt->secsTo(QDateTime::currentDateTime());
    if (secs < 1) return;
    toastService->dynamic(QString("Running... (%1s ago)").arg(secs));
  });
  connect(m_process, &QProcess::finished, this, [this, toastService](int code) {
    auto ms = m_startedAt->msecsTo(QDateTime::currentDateTime());
    m_exited = true;
    m_toastUpdater.stop();
    toastService->clear();
    setNavigationTitle(QString("Done in %1s (exit=%2)").arg(ms / 1e3).arg(code));
    emit runningChanged();
    generateActions();
  });

  startProcess();
}

void ScriptExecutorViewHost::beforePop() {
  auto toastService = context()->services->toastService();
  if (!m_exited && m_process->state() == QProcess::Running) {
    m_process->kill();
    toastService->failure("Script process killed");
  } else {
    toastService->clear();
  }
}

void ScriptExecutorViewHost::killProcess() {
  if (!m_exited && m_process->state() == QProcess::Running) {
    m_process->kill();
    context()->services->toastService()->failure("Script process killed");
  }
}

void ScriptExecutorViewHost::rerun() { startProcess(); }

void ScriptExecutorViewHost::startProcess() {
  auto toastService = context()->services->toastService();
  auto env = QProcessEnvironment::systemEnvironment();

  m_rawOutput.clear();
  m_html.clear();
  m_exited = false;
  m_startedAt.reset();
  emit outputChanged();
  emit runningChanged();

  env.insert("FORCE_COLOR", "1");
  m_process->setEnvironment(env.toStringList());
  m_process->start();
  toastService->dynamic("Running...");
  m_toastUpdater.setInterval(1000);
  m_toastUpdater.start();
}

void ScriptExecutorViewHost::rebuildHtml() {
  m_html = tokenToHtml(m_rawOutput);
  emit outputChanged();
}

void ScriptExecutorViewHost::generateActions() {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();

  if (!m_exited) {
    section->addAction(new StaticAction("Kill process", BuiltinIcon::Droplets,
                                        [this](ApplicationContext *) { killProcess(); }));
  } else {
    section->addAction(
        new StaticAction("Run script again", BuiltinIcon::Undo, [this](ApplicationContext *) { rerun(); }));
  }

  setActions(std::move(panel));
}
