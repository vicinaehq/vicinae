#pragma once
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "qml/manage-fallback-view-host.hpp"
#include <QCoreApplication>

class ManageFallbackCommand : public BuiltinViewCommand<ManageFallbackViewHost> {
  Q_DECLARE_TR_FUNCTIONS(ManageFallbackCommand)

  QString id() const override { return "manage-fallback"; }
  QString name() const override { return tr("Configure Fallback Commands"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("undo").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QString description() const override {
    return tr("Configure what commands are to be presented as fallback options when nothing matches the "
              "search in the root search.");
  }
};
