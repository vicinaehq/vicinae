#pragma once
#include "emoji-command.hpp"
#include "omni-icon.hpp"
#include "single-view-command-context.hpp"
#include "theme.hpp"

class SearchEmojiCommand : public BuiltinViewCommand<EmojiView> {
  QString id() const override { return "search-emojis"; }
  QString name() const override { return "Search Emojis & Symbols"; }
  OmniIconUrl iconUrl() const override {
    return BuiltinOmniIconUrl("emoji").setBackgroundTint(SemanticColor::Red);
  }
};
