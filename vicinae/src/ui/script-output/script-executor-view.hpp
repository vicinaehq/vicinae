#pragma once
#include "builtin_icon.hpp"
#include "layout.hpp"
#include "navigation-controller.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/base-view.hpp"
#include "ui/script-output/script-output-renderer.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "common.hpp"
#include "utils.hpp"
#include <memory>
#include <qnamespace.h>
#include <qtimer.h>

class ScriptExecutorView : public BaseView {
public:
  ScriptExecutorView(const std::vector<QString> &cmdline) : m_cmdline(cmdline) {
    m_renderer->setFocusPolicy(Qt::StrongFocus);
    VStack().add(m_renderer, 1).spacing(0).imbue(this);
  }

protected:
  virtual bool supportsSearch() const override { return false; }

  void beforePop() override { abort(); }

  void abort() {
    const auto toastService = context()->services->toastService();

    if (!m_exited && m_process.state() == QProcess::Running) {
      m_process.kill();
      context()->services->toastService()->failure("Script process killed");
    } else {
      toastService->clear();
    }
  }

  void generateActions() {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();

    if (!m_exited) {
      section->addAction(new StaticAction("Kill process", BuiltinIcon::Droplets,
                                          [this](ApplicationContext *ctx) { abort(); }));
    } else {
      section->addAction(new StaticAction("Run script again", BuiltinIcon::Undo,
                                          [this](ApplicationContext *ctx) { startProcess(); }));
    }

    setActions(std::move(panel));
  }

  void startProcess() {
    const auto toastService = context()->services->toastService();
    auto env = QProcessEnvironment::systemEnvironment();

    assert(m_cmdline.size() != 0);
    m_renderer->clear();
    m_exited = false;
    m_startedAt.reset();
    m_process.setProgram(m_cmdline.at(0));
    m_process.setArguments(m_cmdline | std::views::drop(1) | std::ranges::to<QList>());
    env.insert("FORCE_COLOR", "1");
    m_process.setEnvironment(env.toStringList());
    m_process.start();
    toastService->dynamic("Running...");
    m_toastUpdater.setInterval(1000);
    m_toastUpdater.start();
  }

  void updateProgressToast() {
    if (!m_startedAt) return;
    const auto toastService = context()->services->toastService();
    const auto secondsElapsed = m_startedAt->secsTo(QDateTime::currentDateTime());
    toastService->dynamic(QString("Running... (%1s ago)").arg(secondsElapsed));
  }

  void initialize() override {
    const auto toastService = context()->services->toastService();

    QTimer::singleShot(0, [this]() { m_renderer->setFocus(); });

    connect(&m_process, &QProcess::readyReadStandardOutput, this,
            [this]() { m_renderer->append(m_process.readAllStandardOutput()); });

    connect(&m_process, &QProcess::started, this, [this, toastService]() {
      m_startedAt = QDateTime::currentDateTime();
      generateActions();
    });

    connect(&m_toastUpdater, &QTimer::timeout, this, [this, toastService]() {
      if (!m_startedAt || m_exited) return;
      const auto secondsElapsed = m_startedAt->secsTo(QDateTime::currentDateTime());
      if (secondsElapsed < 1) return;
      toastService->dynamic(QString("Running... (%1s ago)").arg(secondsElapsed));
    });

    connect(&m_process, &QProcess::finished, this, [this, toastService](int code) {
      const auto msElapsed = m_startedAt->msecsTo(QDateTime::currentDateTime());
      m_exited = true;
      m_toastUpdater.stop();
      toastService->clear();
      setNavigationTitle(QString("Done in %1s (exit=%2)").arg(msElapsed / 1e3).arg(code));
      generateActions();
    });

    startProcess();
  }

  std::optional<QDateTime> m_startedAt;
  ScriptOutputRenderer *m_renderer = new ScriptOutputRenderer;
  QTimer m_toastUpdater;
  std::vector<QString> m_cmdline;
  QProcess m_process;
  bool m_exited = false;
};
