#pragma once
#include "generated/tsapi.hpp"
#include "services/app-service/app-service.hpp"

class ExtApplicationService : public tsapi::AbstractApplication {
  using Void = tsapi::Result<void>;

public:
  ExtApplicationService(tsapi::RpcTransport &transport, AppService &appDb)
      : tsapi::AbstractApplication(transport), m_appDb(appDb) {}

  tsapi::Result<std::vector<tsapi::Application>>::Future
  list(const std::optional<std::string> &target) override {
    auto apps = target ? m_appDb.findOpeners(QString::fromStdString(*target)) : m_appDb.list();
    std::vector<tsapi::Application> result;
    result.reserve(apps.size());
    for (const auto &app : apps) {
      result.emplace_back(appToTsapi(*app));
    }
    return tsapi::Result<std::vector<tsapi::Application>>::ok(std::move(result));
  }

  Void::Future open(const std::string &target, const std::optional<std::string> &appId) override {
    QString const qTarget = QString::fromStdString(target);

    if (appId) {
      if (auto app = m_appDb.findById(QString::fromStdString(*appId))) {
        m_appDb.launch(*app, {qTarget});
        return Void::ok();
      }
    }

    if (auto opener = m_appDb.findDefaultOpener(qTarget)) { m_appDb.launch(*opener, {qTarget}); }

    return Void::ok();
  }

  tsapi::Result<tsapi::Application>::Future getDefault(const std::string &target) override {
    auto opener = m_appDb.findDefaultOpener(QString::fromStdString(target));
    if (!opener) return tsapi::Result<tsapi::Application>::fail("No default application found");
    return tsapi::Result<tsapi::Application>::ok(appToTsapi(*opener));
  }

  Void::Future showInFileBrowser(const std::string &target, const bool &select) override {
    m_appDb.showInFileBrowser(target, select);
    return Void::ok();
  }

  tsapi::Result<bool>::Future runInTerminal(const tsapi::RunInTerminalPayload &opts) override {
    LaunchTerminalCommandOptions termOpts;
    std::vector<QString> cmdline;
    cmdline.reserve(opts.cmdline.size());
    for (const auto &s : opts.cmdline) {
      cmdline.emplace_back(QString::fromStdString(s));
    }

    termOpts.hold = opts.hold;
    if (opts.appId) termOpts.appId = QString::fromStdString(*opts.appId);
    if (opts.title) termOpts.title = QString::fromStdString(*opts.title);
    if (opts.workingDirectory) termOpts.workingDirectory = QString::fromStdString(*opts.workingDirectory);

    auto ok = m_appDb.launchTerminalCommand(cmdline, termOpts);
    return tsapi::Result<bool>::ok(ok);
  }

private:
  static tsapi::Application appToTsapi(const ::AbstractApplication &app) {
    return {.id = app.id().toStdString(),
            .name = app.displayName().toStdString(),
            .icon = app.iconUrl().name().toStdString(),
            .path = app.path().string()};
  }

  AppService &m_appDb;
};
