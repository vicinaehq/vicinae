#pragma once
#include "command-database.hpp"
#include "extensions/clipboard/clipboard-history-command.hpp"
#include "../../ui/image/url.hpp"
#include "vicinae.hpp"

class ClipboardExtension : public BuiltinCommandRepository {
public:
  QString id() const override { return "clipboard"; }
  QString displayName() const override { return "Clipboard"; }
  ImageURL iconUrl() const override {
    return ImageURL::builtin("copy-clipboard").setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
  QString description() const override { return "System clipboard integration"; }

  ClipboardExtension() { registerCommand<ClipboardHistoryCommand>(); }
};
