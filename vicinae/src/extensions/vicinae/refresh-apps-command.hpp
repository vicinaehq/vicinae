#pragma once
#include "../../ui/image/url.hpp"
#include "common.hpp"
#include "single-view-command-context.hpp"

class RefreshAppsCommand : public BuiltinCallbackCommand {
  QString id() const override { return "refresh-apps"; }
  QString name() const override { return "Refresh Apps"; }
  QString description() const override {
    return R"("Configure what commands are to be presented as fallback options when nothing matches the
search in the root search.)";
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("redo").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  void execute(const LaunchProps &values, ApplicationContext *ctx) const override;
};
