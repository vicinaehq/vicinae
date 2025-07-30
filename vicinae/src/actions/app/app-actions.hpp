#pragma once
#include "services/app-service/abstract-app-db.hpp"
#include "ui/action-pannel/action.hpp"

class OpenAppAction : public AbstractAction {
  std::shared_ptr<Application> application;
  std::vector<QString> args;

  void execute(ApplicationContext *context) override;

public:
  OpenAppAction(const std::shared_ptr<Application> &app, const QString &title,
                const std::vector<QString> args);
};
