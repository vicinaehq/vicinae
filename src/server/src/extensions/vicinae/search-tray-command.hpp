#pragma once
#include "qml/search-tray-view-host.hpp"
#include "single-view-command-context.hpp"
#include <QCoreApplication>

class SearchTrayCommand : public BuiltinViewCommand<SearchTrayViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SearchTrayCommand)

  QString id() const override { return "search-tray"; }
  QString name() const override { return tr("Search Tray"); }
  QString description() const override {
    return tr("Browse system tray items and trigger their menu actions");
  }
  std::vector<QString> keywords() const override { return {"status", "notifier", "indicator"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::AppWindowList).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
