#pragma once
#include "qml/font-browser-view-host.hpp"
#include "preference.hpp"
#include "single-view-command-context.hpp"
#include <QCoreApplication>

class BrowseFontsCommand : public BuiltinViewCommand<FontBrowserViewHost> {
  Q_DECLARE_TR_FUNCTIONS(BrowseFontsCommand)

  QString id() const override { return "browse"; }
  QString name() const override { return tr("Search Fonts"); };
  std::vector<QString> keywords() const override { return {"browse fonts"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Text).setBackgroundTint(SemanticColor::Orange);
  }

  std::vector<Preference> preferences() const override {
    using Opt = Preference::DropdownData::Option;
    auto defaultAction =
        Preference::makeDropdown("defaultAction", {Opt(tr("Paste"), "paste"), Opt(tr("Copy"), "copy")});
    defaultAction.setDefaultValue("paste");
    defaultAction.setTitle(tr("Default Action"));
    defaultAction.setDescription(tr("The default action to perform on pressing return."));
    return {defaultAction};
  }
};
