#pragma once
#include "services/window-manager/abstract-window-manager.hpp"

namespace Hyprland {

namespace ipc {
struct Workspace;
}

class Workspace : public AbstractWindowManager::AbstractWorkspace {
public:
  QString id() const override;
  QString name() const override;
  bool hasFullScreen() const override;
  std::optional<QString> monitor() const override;

  Workspace(const ipc::Workspace &workspace);

private:
  int m_id = -1;
  QString m_name;
  QString m_monitorName;
  bool m_hasFullScreen = false;
};

} // namespace Hyprland
