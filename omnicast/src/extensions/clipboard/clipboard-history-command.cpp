#include "clipboard-history-command.hpp"
#include "clipboard-history-view.hpp"
#include "service-registry.hpp"
#include "single-view-command-context.hpp"

CommandContext *ClipboardHistoryCommand::createContext(const std::shared_ptr<AbstractCmd> &command) const {
  return new SingleViewCommand<ClipboardHistoryView>(command);
}

void ClipboardHistoryCommand::preferenceValuesChanged(const QJsonValue &value) {
  auto clipman = ServiceRegistry::instance()->clipman();

  qDebug() << "clipboard history preference changes";
}
