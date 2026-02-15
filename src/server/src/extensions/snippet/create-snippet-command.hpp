#include "builtin_icon.hpp"
#include "ui/image/url.hpp"
#include "single-view-command-context.hpp"
#include "qml/qml-snippet-form-view-host.hpp"

class CreateSnippetCommand : public BuiltinViewCommand<QmlSnippetFormViewHost> {
  QString id() const override { return "create"; }
  QString name() const override { return "Create Snippet"; }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Snippets).setBackgroundTint(Omnicast::ACCENT_COLOR);
  }
};
