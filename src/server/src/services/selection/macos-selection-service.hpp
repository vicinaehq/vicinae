#pragma once
#include "services/selection/abstract-selection-service.hpp"
#include <QObject>

class MacosSelectionService : public QObject, public AbstractSelectionService {
  Q_OBJECT

public:
  QFuture<Result> selectedText() override;

private:
  QFuture<Result> copyFromFrontmostApp(int pid);
};
