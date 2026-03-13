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

  using Opt = Preference::DropdownData::Option;

  auto saveDirectoryMode = Preference::makeDropdown(
      "saveDirectoryMode",
      {Opt{"Downloads folder", "downloads"}, Opt{"Home folder", "home"}, Opt{"Custom folder", "custom"}});
  saveDirectoryMode.setDefaultValue("downloads");
  saveDirectoryMode.setTitle("Save Destination");
  saveDirectoryMode.setDescription("Where clipboard entries are saved when using the Save to file action.");
  preferences.emplace_back(saveDirectoryMode);

  auto saveCustomDirectory = Preference::directory("saveCustomDirectory");
  saveCustomDirectory.setTitle("Custom Save Folder");
  saveCustomDirectory.setDescription("Used only when Save Destination is set to Custom folder.");
  saveCustomDirectory.setRequired(false);
  saveCustomDirectory.setDefaultValue(QJsonValue::Null);
  preferences.emplace_back(saveCustomDirectory);

  auto saveFileNameMode = Preference::makeDropdown(
      "saveFileNameMode", {Opt{"Use content preview", "content"}, Opt{"Use timestamp", "timestamp"}});
  saveFileNameMode.setDefaultValue("content");
  saveFileNameMode.setTitle("Save Filename");
  saveFileNameMode.setDescription(
      "Choose whether generated save filenames come from the content or from timestamp.");
  preferences.emplace_back(saveFileNameMode);

  return preferences;
}
