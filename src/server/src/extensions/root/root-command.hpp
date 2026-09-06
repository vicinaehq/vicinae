#pragma once
#include "command/single-view-command-context.hpp"
#include "extensions/root/root-view-host.hpp"

class RootCommand : public BuiltinViewCommand<RootViewHost> {
  QString id() const override { return "root"; }
  QString name() const override { return ""; }
  ImageURL iconUrl() const override { return ImageURL::builtin(BuiltinIcon::Vicinae); }
  QString navigationTitle() const override { return QString(); }
};
