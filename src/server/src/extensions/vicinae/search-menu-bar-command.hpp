#pragma once
#include "qml/menu-bar-search-view-host.hpp"
#include "single-view-command-context.hpp"
#include "theme/colors.hpp"
#include <QCoreApplication>

class SearchMenuBarCommand : public BuiltinViewCommand<MenuBarSearchViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SearchMenuBarCommand)

  QString id() const override { return "search-menu-bar"; }
  QString name() const override { return tr("Search Menu Bar Items"); }
  QString description() const override {
    return tr("Search and run menu bar items of the frontmost application");
  }
  std::vector<QString> keywords() const override { return {"menu", "bar", "items"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
