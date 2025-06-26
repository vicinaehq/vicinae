#include "clipboard-history-command.hpp"
#include "clipboard-history-view.hpp"
#include "service-registry.hpp"
#include "single-view-command-context.hpp"
#include <qjsonobject.h>

CommandContext *ClipboardHistoryCommand::createContext(const std::shared_ptr<AbstractCmd> &command) const {
  return new SingleViewCommand<ClipboardHistoryView>(command);
}

void ClipboardHistoryCommand::preferenceValuesChanged(const QJsonObject &value) const {
  auto clipman = ServiceRegistry::instance()->clipman();

  clipman->setRecordAllOffers(value.value("store-all-offerings").toBool());

  qDebug() << "clipboard history preference changes";
}
