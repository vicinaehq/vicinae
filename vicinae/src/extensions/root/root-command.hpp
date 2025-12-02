#pragma once
#include "single-view-command-context.hpp"
#include "root-search-view2.hpp"

class RootCommand : public BuiltinViewCommand<RootSearchView2> {
  QString id() const override { return "root"; }
  QString name() const override { return ""; }
  ImageURL iconUrl() const override { return ImageURL::builtin("vicinae"); }
  QString navigationTitle() const override { return QString(); }
};
