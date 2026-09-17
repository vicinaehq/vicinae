#include <QCoreApplication>
#include "services/builtin-icon/builtin-icon.hpp"
#include "ui/image/url.hpp"
#include "command/single-view-command-context.hpp"
#include "builtins/snippet/snippet-form-view-host.hpp"

class CreateSnippetCommand : public BuiltinViewCommand<SnippetFormViewHost> {
  QString id() const override { return "create"; }
  QString name() const override {
    return QCoreApplication::translate("CreateSnippetCommand", "Create Snippet");
  }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Snippets)
        .setBackgroundTint(SemanticColor::Orange)
        .setBadge(BuiltinIcon::Plus);
  }
};
