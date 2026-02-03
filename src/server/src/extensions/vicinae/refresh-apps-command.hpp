#pragma once
#include "../../ui/image/url.hpp"
#include "command-controller.hpp"
#include "common.hpp"
#include "single-view-command-context.hpp"

class RefreshAppsCommand : public BuiltinCallbackCommand {
  QString id() const override { return "refresh-apps"; }
  QString name() const override { return "Refresh Apps"; }
  QString description() const override {
    return "Force a refresh of the application database. The database should normally automatically update "
           "itself on changes, but this can help working around some edge cases.";
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("redo").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  void execute(CommandController *controller) const override;
};
