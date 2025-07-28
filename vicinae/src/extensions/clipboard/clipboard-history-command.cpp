#include "clipboard-history-command.hpp"
#include "service-registry.hpp"
#include <qjsonobject.h>

void ClipboardHistoryCommand::preferenceValuesChanged(const QJsonObject &value) const {
  auto clipman = ServiceRegistry::instance()->clipman();

  clipman->setRecordAllOffers(value.value("store-all-offerings").toBool());
  clipman->setMonitoring(value.value("monitoring").toBool());

  qDebug() << "clipboard history preference changes";
}

std::vector<Preference> ClipboardHistoryCommand::preferences() const {
  auto monitoring = Preference::makeCheckbox("monitoring");

  monitoring.setTitle("Clipboard monitoring");
  monitoring.setDescription("Whether clipboard activity is recorded in the history. Every clipboard action "
                            "performed while this is turned off will not be recorded.");
  monitoring.setDefaultValue(true);

  auto storeAllOfferings = Preference::makeCheckbox("store-all-offerings");

  storeAllOfferings.setTitle("Store all offerings");
  storeAllOfferings.setDescription("Store and index alternative mime type offerings. This will "
                                   "increase total storage size, but will refine the search.");
  storeAllOfferings.setDefaultValue(true);

  auto maxStorageSize = Preference::makeText("maximum-storage-size");

  maxStorageSize.setName("maximum-storage-size");
  maxStorageSize.setTitle("Maximum storage size");
  maxStorageSize.setDescription("How much storage can be used to store clipboard history data, in MB.");
  maxStorageSize.setDefaultValue("1000");

  return {monitoring, storeAllOfferings, maxStorageSize};
}
