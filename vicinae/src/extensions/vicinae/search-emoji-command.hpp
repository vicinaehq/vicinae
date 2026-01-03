#pragma once
#include "emoji/emoji.hpp"
#include "preference.hpp"
#include "search-emojis/search-emojis-view.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "services/window-manager/window-manager.hpp"
#include "utils.hpp"

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
    using Opt = Preference::DropdownData::Option;
    auto wm = ServiceRegistry::instance()->windowManager();
    std::vector<Preference> preferences;
    std::vector<Preference::DropdownData::Option> defaultActionOptions;
    QString dflt = "copy";

    if (wm->canPaste()) {
      defaultActionOptions.emplace_back(Opt("Paste", "paste"));
      dflt = "paste";
    }
    defaultActionOptions.emplace_back(Opt("Copy", "copy"));

    auto defaultAction = Preference::makeDropdown("defaultAction", defaultActionOptions);

    defaultAction.setDefaultValue(dflt);
    defaultAction.setTitle("Default Action");
    defaultAction.setDescription("The default action to perform on pressing return. Paste is only available "
                                 "if your environment supports it.");
    preferences.emplace_back(defaultAction);

    {
      std::vector<Preference::DropdownData::Option> opts;

      opts.reserve(1 + emoji::skinTones().size());
      opts.emplace_back(Opt("👍 Default", "default"));

      for (const auto &info : emoji::skinTones()) {
        auto dp = QString("%1 %2")
                      .arg(qStringFromStdView(emoji::applySkinTone("👍", info.tone)))
                      .arg(qStringFromStdView(info.displayName));

        opts.emplace_back(Opt(dp, qStringFromStdView(info.id)));
      }

      auto skinTonePreference = Preference::makeDropdown("skinTone", opts);

      skinTonePreference.setDefaultValue("default");
      skinTonePreference.setTitle("Skin tone");
      skinTonePreference.setDescription("Skin tone to use for relevant emojis.");
      preferences.emplace_back(skinTonePreference);
    }

    return preferences;
  }
};
