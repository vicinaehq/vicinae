#pragma once
#include "common.hpp"
#include "navigation-controller.hpp"
#include "single-view-command-context.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"

class BuiltinUrlCommand : public BuiltinCallbackCommand {
  virtual QUrl url(const ArgumentValues &args) const = 0;

  void execute(CommandController *controller) const override {
    auto ctx = controller->context();
    auto appDb = ctx->services->appDb();
    auto browser = appDb->webBrowser();
    auto &props = controller->launchProps();

    if (!browser) {
      ctx->services->toastService()->setToast("No browser to open the link", ToastStyle::Danger);
      return;
    }

    appDb->launch(*browser, {url(props.arguments).toString()});
    ctx->navigation->showHud("Opened in browser");
    ctx->navigation->clearSearchText();
  }
};
