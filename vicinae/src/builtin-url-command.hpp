#include "single-view-command-context.hpp"
#include "service-registry.hpp"
#include "services/app-service/app-service.hpp"
#include "services/toast/toast-service.hpp"

class BuiltinUrlCommand : public BuiltinCallbackCommand {
  virtual QUrl url() const = 0;

  void execute(ApplicationContext *ctx) const override {
    auto appDb = ctx->services->appDb();

    if (auto browser = appDb->webBrowser()) {
      appDb->launch(*browser, {url().toString()});
      ctx->navigation->showHud("Opened in browser");
      return;
    }

    ctx->services->toastService()->setToast("No browser to open the link", ToastPriority::Danger);
  }
};
