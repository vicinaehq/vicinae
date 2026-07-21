#pragma once
#include "../../ui/image/url.hpp"
#include "command-controller.hpp"
#include "common.hpp"
#include "single-view-command-context.hpp"
#include <QCoreApplication>

class RefreshAppsCommand : public BuiltinCallbackCommand {
  Q_DECLARE_TR_FUNCTIONS(RefreshAppsCommand)

  QString id() const override { return "refresh-apps"; }
  QString name() const override { return tr("Refresh Apps"); }
  QString description() const override {
    return tr("Force a refresh of the application database. The database should normally automatically "
              "update itself on changes, but this can help working around some edge cases.");
  }
  std::vector<QString> keywords() const override { return {"Refresh Apps"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("redo").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  void execute(CommandController &controller) const override;
};
