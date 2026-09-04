#include <QCoreApplication>
#include "builtin_icon.hpp"
#include "preference.hpp"
#include "qml/manage-snippets-view-host.hpp"
#include "single-view-command-context.hpp"
#include "ui/image/url.hpp"

class ManageSnippetsCommand : public BuiltinViewCommand<ManageSnippetsViewHost> {
  Q_DECLARE_TR_FUNCTIONS(ManageSnippetsCommand)

  QString id() const override { return "manage"; }
  QString name() const override { return tr("Manage Snippets"); }
  ImageURL iconUrl() const override {
    return ImageURL(BuiltinIcon::Snippets).setBackgroundTint(SemanticColor::Orange);
  }

  std::vector<Preference> preferences() const override {
    using Opt = Preference::DropdownData::Option;
    auto defaultAction =
        Preference::makeDropdown("defaultAction", {Opt(tr("Paste"), "paste"), Opt(tr("Copy"), "copy")});
    defaultAction.setDefaultValue("paste");
    defaultAction.setTitle(tr("Default Action"));
    defaultAction.setDescription(tr("The default action to perform on pressing return."));
    return {defaultAction};
  }
};
