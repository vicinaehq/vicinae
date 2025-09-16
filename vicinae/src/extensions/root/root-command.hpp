#pragma once
#include "root-search-view.hpp"
#include "single-view-command-context.hpp"

class RootCommand : public BuiltinViewCommand<RootSearchView> {
  QString id() const override { return "root"; }
  QString name() const override { return "Root Search"; }
  ImageURL iconUrl() const override { return ImageURL::builtin("vicinae"); }
  QString navigationTitle() const override { return QString(); }
};
