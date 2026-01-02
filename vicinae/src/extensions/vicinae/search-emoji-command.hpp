#pragma once
#include "search-emojis/search-emojis-view.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "services/window-manager/window-manager.hpp"

class SearchEmojiCommand : public BuiltinViewCommand<EmojiView> {
  QString id() const override { return "search-emojis"; }
  QString name() const override { return "Search Emojis"; }
  QString description() const override {
    return "Search for any emoji to copy it into the clipboard. Also offers pinning, usage "
           "tracking, and custom keyword indexing.";
  }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("emoji").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  std::vector<Preference> preferences() const override {
    auto wm = ServiceRegistry::instance()->windowManager();
    std::vector<Preference> preferences;
    std::vector<Preference::DropdownData::Option> defaultActionOptions;
    QString dflt = "copy";

    if (wm->canPaste()) {
      defaultActionOptions.emplace_back(Preference::DropdownData::Option("Paste", "paste"));
      dflt = "paste";
    }
    defaultActionOptions.emplace_back(Preference::DropdownData::Option("Copy", "copy"));

    auto defaultAction = Preference::makeDropdown("defaultAction", defaultActionOptions);

    defaultAction.setDefaultValue(dflt);
    defaultAction.setTitle("Default Action");
    defaultAction.setDescription("The default action to perform on pressing return. Paste is only available "
                                 "if your environment supports it.");
    preferences.emplace_back(defaultAction);

    return preferences;
  }
};
