#include "clipboard-history-command.hpp"
#include "preference.hpp"

void ClipboardHistoryCommand::preferenceValuesChanged(const QJsonObject &value) const {}

std::vector<Preference> ClipboardHistoryCommand::preferences() const {
  using Opt = Preference::DropdownData::Option;
  auto defaultAction =
      Preference::makeDropdown("defaultAction", {Opt(tr("Paste"), "paste"), Opt(tr("Copy"), "copy")});
  defaultAction.setDefaultValue("paste");
  defaultAction.setTitle(tr("Default Action"));
  defaultAction.setDescription(tr("The default action to perform on pressing return."));
  return {defaultAction};
}
