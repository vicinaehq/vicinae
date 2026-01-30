#include "builtin_icon.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "create-quicklink-command.hpp"

class CreateSnippetCommand : public BuiltinViewCommand<ShortcutFormView> {
  QString id() const override { return "create"; }
  QString name() const override { return "Create Snippet"; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Anchor).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
