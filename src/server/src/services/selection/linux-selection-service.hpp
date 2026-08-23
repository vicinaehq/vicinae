#pragma once
#include "services/selection/abstract-selection-service.hpp"

class LinuxSelectionService : public AbstractSelectionService {
public:
  QFuture<Result> selectedText() override;
};
