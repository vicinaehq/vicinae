#include "qml-script-executor-view-host.hpp"
#include "builtin_icon.hpp"
#include "navigation-controller.hpp"
#include "script/script-output-tokenizer.hpp"
#include "script/script-process.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/action-pannel/action.hpp"

static QString tokenToHtml(const QString &rawOutput) {
  ScriptOutputTokenizer tokenizer(rawOutput);
  QString html;
  html.reserve(rawOutput.size() * 2);
  html += QStringLiteral("<pre style=\"font-size:10.5pt; white-space:pre-wrap; margin:15px;\">");

  bool inSpan = false;

  while (auto tok = tokenizer.next()) {
    if (tok->fmt) {
      if (inSpan) html += QStringLiteral("</span>");
      inSpan = false;

      if (!tok->fmt->reset) {
        QStringList styles;
        if (tok->fmt->fg) styles << QStringLiteral("color:%1").arg(tok->fmt->fg->name());
        if (tok->fmt->bg) styles << QStringLiteral("background:%1").arg(tok->fmt->bg->name());
        if (tok->fmt->italic) styles << QStringLiteral("font-style:italic");
        if (tok->fmt->underline) styles << QStringLiteral("text-decoration:underline");
        if (!styles.isEmpty()) {
          html += QStringLiteral("<span style=\"%1\">").arg(styles.join(';'));
          inSpan = true;
        }
      }
    }

    QString escaped = tok->text.toHtmlEscaped();

    if (tok->url && QUrl(tok->text).isValid()) {
      html += QStringLiteral("<a href=\"%1\">%2</a>").arg(tok->text.toHtmlEscaped(), escaped);
    } else {
      html += escaped;
    }
  }

  if (inSpan) html += QStringLiteral("</span>");
  html += QStringLiteral("</pre>");
  return html;
}

QmlScriptExecutorViewHost::QmlScriptExecutorViewHost(ScriptProcess *process) : m_process(process) {
  process->setParent(this);
}

QUrl QmlScriptExecutorViewHost::qmlComponentUrl() const {
  return QUrl(QStringLiteral("qrc:/Vicinae/ScriptExecutorView.qml"));
}

QVariantMap QmlScriptExecutorViewHost::qmlProperties() const {
  return {{QStringLiteral("host"), QVariant::fromValue(this)}};
}

void QmlScriptExecutorViewHost::initialize() {
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

void QmlScriptExecutorViewHost::beforePop() {
  auto toastService = context()->services->toastService();
  if (!m_exited && m_process->state() == QProcess::Running) {
    m_process->kill();
    toastService->failure("Script process killed");
  } else {
    toastService->clear();
  }
}

void QmlScriptExecutorViewHost::killProcess() {
  if (!m_exited && m_process->state() == QProcess::Running) {
    m_process->kill();
    context()->services->toastService()->failure("Script process killed");
  }
}

void QmlScriptExecutorViewHost::rerun() { startProcess(); }

void QmlScriptExecutorViewHost::startProcess() {
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

void QmlScriptExecutorViewHost::rebuildHtml() {
  m_html = tokenToHtml(m_rawOutput);
  emit outputChanged();
}

void QmlScriptExecutorViewHost::generateActions() {
  auto panel = std::make_unique<ListActionPanelState>();
  auto *section = panel->createSection();

  if (!m_exited) {
    section->addAction(
        new StaticAction("Kill process", BuiltinIcon::Droplets, [this](ApplicationContext *) { killProcess(); }));
  } else {
    section->addAction(
        new StaticAction("Run script again", BuiltinIcon::Undo, [this](ApplicationContext *) { rerun(); }));
  }

  setActions(std::move(panel));
}
