#pragma once
#include "single-view-command-context.hpp"
#include "qml/root-view-host.hpp"

class RootCommand : public BuiltinViewCommand<RootViewHost> {
  QString id() const override { return "root"; }
  QString name() const override { return ""; }
  ImageURL iconUrl() const override { return ImageURL::builtin("vicinae"); }
  QString navigationTitle() const override { return QString(); }
};
