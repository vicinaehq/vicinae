#include "script/script-actions.hpp"
#include "script-command.hpp"
#include "script/script-command-file.hpp"
#include "script/script-process.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/app-service/app-service.hpp"
#include "service-registry.hpp"
#include "navigation-controller.hpp"
#include "ui/script-output/script-executor-view.hpp"

ScriptExecutorAction::ScriptExecutorAction(const std::shared_ptr<ScriptCommandFile> &file,
                                           std::optional<script_command::OutputMode> mode)
    : m_file(file), m_outputModeOverride(mode) {}

QString ScriptExecutorAction::title() const { return "Run script"; }

std::optional<ImageURL> ScriptExecutorAction::icon() const { return ScriptCommandFile::defaultIcon(); }

void ScriptExecutorAction::executeOneLine(const ScriptCommandFile &script, const std::vector<QString> &args,
                                          const std::function<void(bool ok, QString line)> &callback) {
  auto process = new ScriptProcess(script, args);

  process->start();

  // times out after 10s
  QTimer::singleShot(10000, process, [process]() {
    if (process->state() == QProcess::Running) { process->kill(); }
  });

  QObject::connect(process, &QProcess::errorOccurred, [process, callback]() {
    callback(false, QString());
    process->deleteLater();
  });
  QObject::connect(process, &QProcess::finished, [process, callback](int exit, QProcess::ExitStatus status) {
    QString line = process->readAllStandardOutput().split('\n').first();
    callback(exit == 0, line);
    process->deleteLater();
  });
}

// FIXME: in the future we should make it so that script commands are considered as regular
// commands and have a properly defined lifetime using an execution context, so that if another command is
// activated we can automatically cancel any pending script execution. The current architecture does not
// allow this, so this will have to do for now.
void ScriptExecutorAction::execute(ApplicationContext *ctx) {
  const auto error = m_file->reload();

  if (error) {
    ctx->services->toastService()->failure(QString("Failed to parse script: %1").arg(error->c_str()));
    return;
  }

  const bool needsConfirm = m_file->data().needsConfirmation;
  const auto outputMode = m_outputModeOverride.value_or(m_file->data().mode);
  const auto runScript = [script = m_file, ctx, outputMode]() {
    const auto args = ctx->navigation->unnamedCompletionValues();

    using Mode = script_command::OutputMode;

    switch (outputMode) {
    case Mode::Full:
      ctx->navigation->pushView(new ScriptExecutorView(new ScriptProcess(*script, args)));
      ctx->navigation->setNavigationIcon(script->icon());
      break;
    case Mode::Silent:
      executeOneLine(*script, args, [ctx](bool ok, const QString &line) {
        if (!ok) {
          ctx->services->toastService()->failure(line.isEmpty() ? "Failed to execute script" : line);
          return;
        }

        ctx->navigation->showHud(line.isEmpty() ? "Script executed" : line);
        ctx->navigation->clearSearchText();
      });
      break;
    case Mode::Compact:
      executeOneLine(*script, args, [ctx](bool ok, const QString &line) {
        const auto toastService = ctx->services->toastService();
        ToastStyle style = ok ? ToastStyle::Success : ToastStyle::Danger;

        if (line.isEmpty()) {
          toastService->setToast(ok ? "Script executed" : "Script execution failed", style);
        } else {
          toastService->setToast(line, style);
        }
      });
      break;
    case Mode::Terminal:
      if (!ctx->services->appDb()->launchTerminalCommand(script->createCommandLine(args), {.hold = true})) {
        ctx->services->toastService()->failure("Failed to execute script");
      } else {
        ctx->navigation->closeWindow();
        ctx->navigation->clearSearchText();
      }
      break;
    case Mode::Inline:
      executeOneLine(*script, args, [id = std::string(script->id()), ctx](bool ok, const QString &line) {
        if (!ok) {
          ctx->services->toastService()->failure(line.isEmpty() ? "Script exited with error code" : line);
          return;
        }

        ctx->services->scriptDb()->metadata()->saveRun(id, line.toStdString());
        emit ctx->services->rootItemManager()->subtitleChanged();
      });
      break;
    }
  };

  if (needsConfirm) {
    ctx->navigation->confirmAlert("Are you sure", "This script needs confirmation to execute", runScript);
    return;
  }

  runScript();
}
