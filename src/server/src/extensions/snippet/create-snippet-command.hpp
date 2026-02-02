#include "builtin_icon.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "create-snippet-view.hpp"

class CreateSnippetCommand : public BuiltinViewCommand<CreateSnippetView> {
  QString id() const override { return "create"; }
  QString name() const override { return "Create Snippet"; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Snippets).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
