#pragma once
#include "actions/files/file-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "script-command.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/script-command/script-command-service.hpp"
#include "services/app-service/app-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "services/toast/toast-service.hpp"
#include "ui/script-output/script-executor-view.hpp"
#include "ui/settings-item-info/settings-item-info.hpp"
#include "ui/toast/toast.hpp"
#include "utils.hpp"
#include <QtCore>
#include <qcontainerfwd.h>
#include <qprocess.h>
#include <ranges>

class ScriptExecutorAction : public AbstractAction {
public:
  ScriptExecutorAction(const std::shared_ptr<ScriptCommandFile> &file,
                       std::optional<script_command::OutputMode> mode = std::nullopt)
      : m_file(file), m_outputModeOverride(mode) {}

  void execute(ApplicationContext *ctx) override {
    const auto error = m_file->reload();

    if (error) {
      ctx->services->toastService()->failure(QString("Failed to parse script: %1").arg(error.value()));
      return;
    }

    bool needsConfirm = m_file->data().needsConfirmation;

    const auto outputMode = m_outputModeOverride.value_or(m_file->data().mode);
    const auto runScript = [script = m_file, ctx, outputMode]() {
      auto process = new ScriptProcess(*script, ctx->navigation->unnamedCompletionValues());

      using Mode = script_command::OutputMode;

      std::vector<QString> cmdline;

      switch (outputMode) {
      case Mode::Full:
        ctx->navigation->pushView(new ScriptExecutorView(process));
        ctx->navigation->setNavigationIcon(script->icon());
        break;
      case Mode::Silent:
        executeOneLine(process, [ctx](bool ok, const QString &line) {
          if (line.isEmpty()) {
            ctx->navigation->showHud(ok ? "Script executed" : "Script execution failed");
          } else {
            ctx->navigation->showHud(line);
          }
          ctx->navigation->clearSearchText();
        });
        break;
      case Mode::Compact:
        executeOneLine(process, [ctx](bool ok, const QString &line) {
          const auto toastService = ctx->services->toastService();
          ToastStyle style = ok ? ToastStyle::Success : ToastStyle::Danger;

          if (line.isEmpty()) {
            toastService->setToast(ok ? "Script executed" : "Script execution failed", style);
          } else {
            toastService->setToast(line, style);
          }

          ctx->services->toastService()->setToast(line.isEmpty() ? "Script executed" : line);
        });
        break;
      case Mode::Terminal:
        ctx->services->appDb()->launchTerminalCommand(cmdline, {.hold = true});
        ctx->navigation->closeWindow();
        break;
      case Mode::Inline:
        executeOneLine(process, [id = std::string(script->id()), ctx](bool ok, const QString &line) {
          if (!ok) {
            ctx->services->toastService()->failure("Script exited with error code");
            return;
          }

          ScriptMetadataStore store;
          store.saveRun(id, line.toStdString());
          emit ctx->services->rootItemManager()->metadataChanged();
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

  QString title() const override { return "Run script"; }

private:
  // FIXME: in the future we should make it so that script commands are considered as regular
  // commands and have a properly defined lifetime using an execution context, so that if another command is
  // activated we can automatically cancel any pending script execution. The current architecture does not
  // allow this, so this will have to do for now.
  static void executeOneLine(QProcess *process, const std::function<void(bool ok, QString line)> &callback) {
    process->start();

    // times out after 10s
    QTimer::singleShot(10000, process, [process]() {
      if (process->state() == QProcess::Running) { process->kill(); }
    });

    QObject::connect(process, &QProcess::finished,
                     [process, callback](int exit, QProcess::ExitStatus status) {
                       QString line = process->readAllStandardOutput().split('\n').first();
                       callback(status == QProcess::ExitStatus::NormalExit, line);
                       process->deleteLater();
                     });
  }

  std::shared_ptr<ScriptCommandFile> m_file;
  std::optional<script_command::OutputMode> m_outputModeOverride;
};

class ScriptRootItem : public RootItem {
  QString displayName() const override { return m_file->data().title.c_str(); }

  QString subtitle() const override { return m_file->packageName().c_str(); }

  QString typeDisplayName() const override { return "Script"; }

  ArgumentList arguments() const override {
    ArgumentList args;
    args.reserve(m_file->data().arguments.size());

    for (const auto &[idx, arg] : m_file->data().arguments | std::views::enumerate) {
      CommandArgument cmdArg;

      cmdArg.required = !arg.optional;
      cmdArg.name = QString("argument%1").arg(idx);
      cmdArg.placeholder =
          arg.placeholder.transform([](auto &&s) { return QString::fromStdString(s); }).value_or(cmdArg.name);

      using T = script_command::ArgumentType;

      switch (arg.type) {
      case T::Text:
        cmdArg.type = CommandArgument::Text;
        break;
      case T::Password:
        cmdArg.type = CommandArgument::Password;
        break;
      case T::Dropdown:
        cmdArg.type = CommandArgument::Dropdown;
        break;
      }

      args.emplace_back(cmdArg);
    }

    return args;
  }

  QWidget *settingsDetail(const QJsonObject &preferences) const override {
    std::vector<std::pair<QString, QString>> args;

    args.reserve(6);
    args.emplace_back(
        std::pair{"Mode", qStringFromStdView(script_command::outputModeToString(m_file->data().mode))});
    args.emplace_back(std::pair{"Path", compressPath(m_file->path()).c_str()});

    if (const auto author = m_file->data().author) {
      args.emplace_back(std::pair{"Author", author.value().c_str()});
    }

    return new SettingsItemInfo(
        args, m_file->data().description.transform([](auto &&s) { return QString::fromStdString(s); }));
  }

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();
    auto editor = ctx->services->appDb()->textEditor();
    auto exec = new ScriptExecutorAction(m_file);

    section->addAction(new DefaultActionWrapper(uniqueId(), exec));

    if (editor) { section->addAction(new OpenFileAction(m_file->path(), editor)); }

    return panel;
  }

  AccessoryList accessories() const override { return {{.text = "Script"}}; }

  EntrypointId uniqueId() const override { return EntrypointId("scripts", std::string{m_file->id()}); };

  ImageURL iconUrl() const override { return m_file->icon(); }

  std::vector<QString> keywords() const override { return Utils::toQStringVec(m_file->data().keywords); }

public:
  ScriptRootItem(const std::shared_ptr<ScriptCommandFile> &file) : m_file(file) {}

private:
  std::shared_ptr<ScriptCommandFile> m_file;
};

class ScriptRootProvider : public RootProvider {
public:
  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_service.scripts() | std::views::transform([](auto item) -> std::shared_ptr<RootItem> {
             return std::make_shared<ScriptRootItem>(std::move(item));
           }) |
           std::ranges::to<std::vector>();
  }

  Type type() const override { return Type::GroupProvider; }

  ImageURL icon() const override { return ImageURL::emoji("🤖"); }

  QString displayName() const override { return "Script Commands"; }

  QString uniqueId() const override { return "scripts"; }

  PreferenceList preferences() const override { return {}; }

  void preferencesChanged(const QJsonObject &preferences) override {}

  ScriptRootProvider() {
    connect(&m_service, &ScriptCommandService::scriptsChanged, this, [this]() { emit itemsChanged(); });
  }

private:
  ScriptCommandService m_service;
};
