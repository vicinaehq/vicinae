#include "clipboard-history-command.hpp"
#include <qjsonobject.h>

void ClipboardHistoryCommand::preferenceValuesChanged(const QJsonObject &value) const {}

std::vector<Preference> ClipboardHistoryCommand::preferences() const { return {}; }
