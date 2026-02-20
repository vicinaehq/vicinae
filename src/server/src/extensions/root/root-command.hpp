#pragma once
#include "single-view-command-context.hpp"
#include "qml/qml-root-view-host.hpp"

class RootCommand : public BuiltinViewCommand<QmlRootViewHost> {
  QString id() const override { return "root"; }
  QString name() const override { return ""; }
  ImageURL iconUrl() const override { return ImageURL::builtin("vicinae"); }
  QString navigationTitle() const override { return QString(); }
};
