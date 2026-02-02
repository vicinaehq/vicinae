#include "builtin_icon.hpp"
#include "manage-snippets-view.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"

class ManageSnippetsCommand : public BuiltinViewCommand<ManageSnippetsView> {
  QString id() const override { return "manage"; }
  QString name() const override { return "Manage Snippets"; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Snippets).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
