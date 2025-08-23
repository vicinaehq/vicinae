#pragma once
#include "common.hpp"
#include "navigation-controller.hpp"
#include "single-view-command-context.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"

class BuiltinUrlCommand : public BuiltinCallbackCommand {
  virtual QUrl url(const ArgumentValues &args) const = 0;

  void execute(const LaunchProps &props, ApplicationContext *ctx) const override {
    auto appDb = ctx->services->appDb();

    if (auto browser = appDb->webBrowser()) {
      appDb->launch(*browser, {url(props.arguments).toString()});
      ctx->navigation->showHud("Opened in browser");
      return;
    }

    ctx->services->toastService()->setToast("No browser to open the link", ToastPriority::Danger);
  }
};
