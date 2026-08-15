#pragma once
#include "command-database.hpp"
#include "single-view-command-context.hpp"
#include "vicinae.hpp"
#include "markdown-showcase-command.hpp"
#include <QCoreApplication>

class InternalExtension : public BuiltinCommandRepository {
  Q_DECLARE_TR_FUNCTIONS(InternalExtension)

  QString id() const override { return "internal"; }
  QString displayName() const override { return tr("Internal Commands"); }
  QString description() const override { return tr("Internal Commands"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::MagnifyingGlass).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

public:
  InternalExtension() {
    registerCommand<MarkdownShowcase>();
  }
};
