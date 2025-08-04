#include "../../ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "create-quicklink-command.hpp"
#include "theme.hpp"

class CreateShortcutCommand : public BuiltinViewCommand<BookmarkFormView> {
  QString id() const override { return "create"; }
  QString name() const override { return "Create Shortcut"; }
  ImageURL iconUrl() const override {
    return BuiltinOmniIconUrl("link").setBackgroundTint(SemanticColor::Red);
  }
};
