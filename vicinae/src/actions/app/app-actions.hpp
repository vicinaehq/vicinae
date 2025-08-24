#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "ui/action-pannel/action.hpp"

class OpenAppAction : public AbstractAction {
  std::shared_ptr<Application> application;
  std::vector<QString> args;
  bool m_clearSearch = false;

  void execute(ApplicationContext *context) override;

public:
  void setClearSearch(bool value) { m_clearSearch = value; }

  OpenAppAction(const std::shared_ptr<Application> &app, const QString &title,
                const std::vector<QString> args);
};
