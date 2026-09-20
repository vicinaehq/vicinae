#pragma once
#include "glyph/emoji.hpp"
#include "builtins/vicinae/emoji-preferences.hpp"
#include "command/typed-command.hpp"
#include "builtins/vicinae/emoji-grid-view-host.hpp"
#include "ui/image/url.hpp"
#include "command/single-view-command-context.hpp"
#include "services/paste/paste-service.hpp"
#include "utils.hpp"
#include <QCoreApplication>

class SearchEmojiCommand : public TypedViewCommand<EmojiGridViewHost, EmojiPreferences> {
  Q_DECLARE_TR_FUNCTIONS(SearchEmojiCommand)

  QString id() const override { return "search-emojis"; }
  QString name() const override { return tr("Search Emojis & Symbols"); }
  QString description() const override { return tr("Search for any emoji or symbol"); }
  ImageURL iconUrl() const override {
    return ImageURL::builtin(BuiltinIcon::Emoji).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
