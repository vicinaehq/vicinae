#include "clipboard-history-command.hpp"
#include "service-registry.hpp"
#include <qjsonobject.h>
#include "services/paste/paste-service.hpp"

void ClipboardHistoryCommand::preferenceValuesChanged(const QJsonObject &value) const {}

std::vector<Preference> ClipboardHistoryCommand::preferences() const {
  auto paste = ServiceRegistry::instance()->pasteService();
  std::vector<Preference> preferences;

  std::vector<Preference::DropdownData::Option> defaultActionOptions;
  QString dflt = "copy";

  if (paste->supportsPaste()) {
    defaultActionOptions.emplace_back(Preference::DropdownData::Option{"Paste", "paste"});
    dflt = "paste";
  }
  defaultActionOptions.emplace_back(Preference::DropdownData::Option{"Copy", "copy"});

  auto defaultAction = Preference::makeDropdown("defaultAction", defaultActionOptions);

  defaultAction.setDefaultValue(dflt);
  defaultAction.setTitle("Default Action");
  defaultAction.setDescription("The default action to perform on pressing return. Paste is only available "
                               "if your environment supports it.");
  preferences.emplace_back(defaultAction);

  return preferences;
}
