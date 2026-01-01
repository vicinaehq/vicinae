#pragma once
#include "actions/files/file-actions.hpp"
#include "actions/root-search/root-search-actions.hpp"
#include "script-command.hpp"
#include "script/script-actions.hpp"
#include "script/script-command-file.hpp"
#include "services/root-item-manager/root-item-manager.hpp"
#include "services/script-command/script-command-service.hpp"
#include "services/app-service/app-service.hpp"
#include "navigation-controller.hpp"
#include "service-registry.hpp"
#include "ui/settings-item-info/settings-item-info.hpp"
#include "utils.hpp"
#include <QProcess>
#include <ranges>

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

  ScriptRootProvider(ScriptCommandService &service) : m_service(service) {
    connect(&m_service, &ScriptCommandService::scriptsChanged, this, [this]() { emit itemsChanged(); });
  }

private:
  ScriptCommandService &m_service;
};
