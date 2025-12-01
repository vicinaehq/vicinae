#pragma once
#include "search-emojis/search-emojis-view.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"

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
};
