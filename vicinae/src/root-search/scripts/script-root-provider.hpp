#include "actions/files/file-actions.hpp"
#include "argument.hpp"
#include "common.hpp"
#include "navigation-controller.hpp"
#include "script-command.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/script-command/script-command-service.hpp"
#include "ui/action-pannel/action.hpp"
#include "ui/views/base-view.hpp"
#include "utils.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"
#include <algorithm>
#include <cassert>
#include <qnamespace.h>
#include <qplaintextedit.h>
#include <qprocess.h>
#include <qstringliteral.h>
#include <qstringview.h>
#include <qtextbrowser.h>
#include <qtextdocument.h>
#include <qtextedit.h>
#include <qtextformat.h>
#include <ranges>
#include "ui/script-output/script-executor-view.hpp"

class ScriptExecutorAction : public AbstractAction {
public:
  ScriptExecutorAction(const std::filesystem::path &path,
                       std::optional<script_command::OutputMode> mode = std::nullopt)
      : m_path(path), m_outputModeOverride(mode) {}

  void execute(ApplicationContext *ctx) override {
    const auto result = ScriptCommandFile::fromFile(m_path);

    if (!result) {
      ctx->services->toastService()->failure(
          QString("Failed to parse script: %1").arg(result.error().c_str()));
      return;
    }

    auto cmdline = result->data().exec;

    assert(!cmdline.empty());

    if (QStandardPaths::findExecutable(cmdline.front().c_str()).isEmpty()) {
      ctx->services->toastService()->failure(QString("Unknown executable %1").arg(cmdline.front()));
      return;
    }

    cmdline.emplace_back(m_path);

    for (const auto &value : ctx->navigation->completionValues()) {
      cmdline.emplace_back(value.second.toStdString());
    }

    using Mode = script_command::OutputMode;

    switch (m_outputModeOverride.value_or(result->data().mode)) {
    case Mode::Full:
      ctx->navigation->pushView(new ScriptExecutorView(cmdline));
      ctx->navigation->setNavigationIcon(result->icon());
      break;
    case Mode::Silent:
      executeOneLine(Utils::toQStringVec(cmdline),
                     [ctx](const QString &line) { ctx->navigation->showHud(line); });
      break;
    case Mode::Compact:
      executeOneLine(Utils::toQStringVec(cmdline),
                     [ctx](const QString &line) { ctx->services->toastService()->setToast(line); });
      break;
    case Mode::Inline:
      break;
    }
  }

  void executeOneLine(const std::vector<QString> &cmdline,
                      const std::function<void(QString line)> &callback) {
    auto process = new QProcess;
    process->setProgram(cmdline.at(0));
    process->setArguments(cmdline | std::views::drop(1) | std::ranges::to<QList>());
    process->start();

    QObject::connect(process, &QProcess::readyReadStandardOutput, [process, callback]() {
      QString line = process->readAllStandardOutput().split('\n').first();
      callback(line);
    });

    QTimer::singleShot(10000, process, [process]() {
      if (process->state() == QProcess::Running) { process->kill(); }
    });

    QObject::connect(process, &QProcess::finished, [process]() { process->deleteLater(); });
  }

private:
  std::filesystem::path m_path;
  std::optional<script_command::OutputMode> m_outputModeOverride;
};

class ScriptRootItem : public RootItem {
  QString displayName() const override { return m_file.data().title.c_str(); }

  QString subtitle() const override { return m_file.packageName().c_str(); }

  QString typeDisplayName() const override { return "Script"; }

  ArgumentList arguments() const override {
    ArgumentList args;
    args.reserve(m_file.data().arguments.size());

    for (const auto &[idx, arg] : m_file.data().arguments | std::views::enumerate) {
      CommandArgument cmdArg;

      cmdArg.required = !arg.optional;
      cmdArg.name = QString("argument%1").arg(idx);
      cmdArg.placeholder = arg.placeholder.c_str();

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

  std::unique_ptr<ActionPanelState> newActionPanel(ApplicationContext *ctx,
                                                   const RootItemMetadata &metadata) const override {
    auto panel = std::make_unique<ListActionPanelState>();
    auto section = panel->createSection();
    auto editor = ctx->services->appDb()->textEditor();

    section->addAction(new ScriptExecutorAction(m_file.path()));

    if (editor) { section->addAction(new OpenFileAction(m_file.path(), editor)); }

    return panel;
  }

  AccessoryList accessories() const override { return {{.text = "Script"}}; }

  EntrypointId uniqueId() const override { return EntrypointId("scripts", m_file.path()); };

  ImageURL iconUrl() const override { return m_file.icon(); }

  std::vector<QString> keywords() const override { return Utils::toQStringVec(m_file.data().keywords); }

public:
  ScriptRootItem(ScriptCommandFile &&file) : m_file(std::move(file)) {}

private:
  ScriptCommandFile m_file;
};

class ScriptRootProvider : public RootProvider {
public:
  std::vector<std::shared_ptr<RootItem>> loadItems() const override {
    return m_service.scanAll() | std::views::transform([](auto item) -> std::shared_ptr<RootItem> {
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

  ScriptRootProvider() = default;

private:
  ScriptCommandService m_service;
};
