#pragma once
#include "glyph/emoji.hpp"
#include "preference.hpp"
#include "qml/emoji-grid-view-host.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "services/paste/paste-service.hpp"
#include "utils.hpp"
#include <QCoreApplication>

class SearchEmojiCommand : public BuiltinViewCommand<EmojiGridViewHost> {
  Q_DECLARE_TR_FUNCTIONS(SearchEmojiCommand)

  QString id() const override { return "search-emojis"; }
  QString name() const override { return tr("Search Emojis & Symbols"); }
  QString description() const override { return tr("Search for any emoji or symbol"); }
  std::vector<QString> keywords() const override { return {"Search Emojis & Symbols"}; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("emoji").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }

  std::vector<Preference> preferences() const override {
    using Opt = Preference::DropdownData::Option;
    auto paste = ServiceRegistry::instance()->pasteService();
    std::vector<Preference> preferences;
    std::vector<Preference::DropdownData::Option> defaultActionOptions;
    QString dflt = "copy";

    if (paste->supportsPaste()) {
      defaultActionOptions.emplace_back(Opt(tr("Paste"), "paste"));
      dflt = "paste";
    }
    defaultActionOptions.emplace_back(Opt(tr("Copy"), "copy"));

    auto defaultAction = Preference::makeDropdown("defaultAction", defaultActionOptions);

    defaultAction.setDefaultValue(dflt);
    defaultAction.setTitle(tr("Default Action"));
    defaultAction.setDescription(tr("The default action to perform on pressing return. Paste is only "
                                    "available if your environment supports it."));
    preferences.emplace_back(defaultAction);

    {
      std::vector<Preference::DropdownData::Option> opts;

      opts.reserve(emoji::skinTones().size());
      // opts.emplace_back(Opt("👍 Default", "default"));

      for (const auto &info : emoji::skinTones()) {
        auto dp = QString("%1 %2")
                      .arg(qStringFromStdView(emoji::applySkinTone("👍", info.tone)))
                      .arg(qStringFromStdView(info.displayName));

        opts.emplace_back(Opt(dp, qStringFromStdView(info.id)));
      }

      auto skinTonePreference = Preference::makeDropdown("skinTone", opts);

      skinTonePreference.setDefaultValue("default");
      skinTonePreference.setTitle(tr("Skin tone"));
      skinTonePreference.setDescription(tr("Skin tone to use for relevant emojis."));
      preferences.emplace_back(skinTonePreference);
    }

    return preferences;
  }
};
