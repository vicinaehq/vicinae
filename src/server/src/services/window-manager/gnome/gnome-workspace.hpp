#pragma once
#include "services/window-manager/abstract-window-manager.hpp"
#include <QJsonObject>

namespace Gnome {
class Workspace : public AbstractWindowManager::AbstractWorkspace {
public:
  QString id() const override;
  QString name() const override;
  QString monitor() const override;
  bool hasFullScreen() const override;

  Workspace(const QJsonObject &json);

private:
  int m_id;
  QString m_name;
  QString m_monitorId;
  bool m_hasFullScreen;
};
} // namespace Gnome
