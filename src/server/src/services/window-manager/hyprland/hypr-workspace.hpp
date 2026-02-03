#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include <qjsonobject.h>

namespace Hyprland {
class Workspace : public AbstractWindowManager::AbstractWorkspace {
public:
  QString id() const override;
  QString name() const override;
  bool hasFullScreen() const override;
  QString monitor() const override;

  Workspace(const QJsonObject &json);

private:
  int m_monitorId = -1;
  int m_id = -1;
  QString m_name;
  bool m_hasFullScreen = false;
};
}; // namespace Hyprland
