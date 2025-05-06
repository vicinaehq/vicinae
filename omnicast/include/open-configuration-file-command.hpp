#pragma once
#include "app.hpp"
#include "command.hpp"
#include "omnicast.hpp"
#include "service-registry.hpp"
#include "ui/toast.hpp"

class OpenConfigurationFileCommand : public CommandContext {
public:
  void load(const LaunchProps &props) override {
    auto appDb = ServiceRegistry::instance()->appDb();
    auto configFile = Omnicast::configDir() / "omnicast.json";

    if (auto opener = appDb->findBestOpener(configFile.c_str())) {
      appDb->launch(*opener, {configFile.c_str()});
      requestWindowClose();
      return;
    }

    if (auto browser = appDb->textEditor()) {
      appDb->launch(*browser, {configFile.c_str()});
      app()->closeWindow();
      return;
    }

    requestToast("No opener available for this file", ToastPriority::Danger);
  }

  OpenConfigurationFileCommand(AppWindow *app, const std::shared_ptr<AbstractCmd> &command)
      : CommandContext(app, command) {}
};
