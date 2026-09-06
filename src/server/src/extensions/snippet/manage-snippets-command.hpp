#include <QCoreApplication>
#include "services/builtin-icon/builtin-icon.hpp"
#include "extensions/snippet/manage-snippets-view-host.hpp"
#include "ui/image/url.hpp"
#include "command/single-view-command-context.hpp"

class ManageSnippetsCommand : public BuiltinViewCommand<ManageSnippetsViewHost> {
  QString id() const override { return "manage"; }
  QString name() const override {
    return QCoreApplication::translate("ManageSnippetsCommand", "Manage Snippets");
  }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Snippets).setBackgroundTint(SemanticColor::Orange);
  }
};
